#include "ApplicationRSSIMeasServer.hpp"
#include "Log.hpp"
#include "ApplicationInterface.hpp"
#include <functional>
#include "CommandInterpreter.hpp"

#define MEASUREMENTS_PER_CLIENT   200
#define MEASUREMENT_INTERVALL     1000
#define PING_INTERVALL            1000

ApplicationRSSIMeasServer::ApplicationRSSIMeasServer(uint16_t port)
 : m_Port(port)
 , m_WebSocketServer(m_Port)
 , m_ClientList()
 , m_NextPing(0)
 , m_NextStart(0)
 , m_LastMeasurement()
 , m_RandomWalkState(STATE_INIT)
 , m_NextRandomWalkState(STATE_INIT)
 , m_SampleWalkMeasurements()
 , m_SampleWalkSample(0)
 , m_SampleWalkState(STATE_SAMPLE_INIT)
{
  m_LastMeasurement[0] = m_LastMeasurement[1] = 0x80000000; // INT_MIN

  logDebug("Start Application RSSIMeasServer with WebSocket on port %d\n", m_Port);
#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
  logDebug("WebSocketServer runs in asynchronous mode!\n");
#endif
  m_WebSocketServer.begin();
  m_WebSocketServer.onEvent(std::bind(&ApplicationRSSIMeasServer::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  CommandInterpreter::get()->addCommand("appPrint", std::bind(&ApplicationRSSIMeasServer::printClientData, this, std::placeholders::_1));
  CommandInterpreter::get()->addCommand("appStart", std::bind(&ApplicationRSSIMeasServer::appStartCommand, this, std::placeholders::_1));
  CommandInterpreter::get()->addCommand("appStop", std::bind(&ApplicationRSSIMeasServer::appStopCommand, this, std::placeholders::_1));
}

ApplicationRSSIMeasServer::~ApplicationRSSIMeasServer()
{
  CommandInterpreter::get()->deleteCommand("appPrint");
  CommandInterpreter::get()->deleteCommand("appStart");
  CommandInterpreter::get()->deleteCommand("appStop");
}

void ApplicationRSSIMeasServer::loop()
{
  std::list<RSSIClientData_t>::iterator it = m_ClientList.begin();
  if (millis() > m_NextPing)
  {
    while (it != m_ClientList.end())
    {
      if (it->connected == true)
      {
        if (m_WebSocketServer.sendPing(it->clientNum) == false)
        {
          logWarning("[APP RSSIMeas] Detected connection loss to client %d. Force disconnect.\n", it->clientNum);
          m_WebSocketServer.disconnect(it->clientNum);
#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
          // Manual fired disconnect event is needed in async mode
          webSocketEvent(it->clientNum, WStype_DISCONNECTED, NULL, 0);
#endif
          continue;
        }
      }
      it++;
    }
    m_NextPing = millis() + PING_INTERVALL;
  }

  Gondola *gondola = ApplicationInterface::get()->getGondola();

  // randomWalk(gondola, 5.0);
  sampleWalk(gondola, 5.0);

#if (WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
  m_WebSocketServer.loop();
#endif
}

void ApplicationRSSIMeasServer::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
 switch(type)
 {
  case WStype_DISCONNECTED:
  {
    logDebug("[APP RSSIMeas][%u] Disconnected!\n", num);
    RSSIClientData_t *clientData = getClientData(num);
    clientData->connected = false;
    break;
  }

  case WStype_CONNECTED:
  {
    IPAddress ip = m_WebSocketServer.remoteIP(num);
    logDebug("[APP RSSIMeas][%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
		// send message back to client
		m_WebSocketServer.sendTXT(num, "[APP RSSIMeas] Connected");
    m_RandomWalkState = STATE_INIT;
  }
  break;

  case WStype_TEXT:
    // logDebug("[%u] get Text: %s\n", num, payload);
    break;

  case WStype_BIN:
  {
    // logDebug("[%u] get binary length: %u\n", num, length);
    ApplicationRSSIMeasCmd_t cmd = static_cast<ApplicationRSSIMeasCmd_t>(payload[0]);
    switch (cmd)
    {
      case RSSI_MEAS_C_REG:
      {
        RSSIClientData_t *clientData = getClientData(payload + 1);
        // Client is already registered
        if (clientData != NULL)
        {
          clientData->clientNum = num;
          clientData->connected = true;
          clientData->clientIP = m_WebSocketServer.remoteIP(num);
        }
        else
        {
          RSSIClientData_t newClientData;
          newClientData.clientNum = num;
          newClientData.clientIP = m_WebSocketServer.remoteIP(num);
          newClientData.mac[0] = payload[1];
          newClientData.mac[1] = payload[2];
          newClientData.mac[2] = payload[3];
          newClientData.mac[3] = payload[4];
          newClientData.mac[4] = payload[5];
          newClientData.mac[5] = payload[6];
          newClientData.connected = true;
          m_ClientList.push_back(newClientData);
        }
        break;
      }
      case RSSI_MEAS_C_MEAS:
      {
        b4Converter_t converter;
        converter.b[0] = payload[1];
        converter.b[1] = payload[2];
        converter.b[2] = payload[3];
        converter.b[3] = payload[4];
        int32_t rssi = converter.i;
        logDebug("[APP RSSIMeas][%d][t:%d] Got new RSSI Measurement RSSI=%d\n", num, millis(), rssi);
        RSSIClientData_t *clientData = getClientData(num);
        RSSIMeasurement_t measurement;
        Gondola* gondola = ApplicationInterface::get()->getGondola();
        if (gondola != NULL)
        {
          measurement.gondolaPos = gondola->getCurrentPosition();
          measurement.clientRSSI = rssi;
          clientData->measurementList.push_back(measurement);
          if (clientData->measurementList.size() > MEASUREMENTS_PER_CLIENT)
          {
            clientData->measurementList.pop_front();
          }
        }
        // Random walk
        m_LastMeasurement[1] = m_LastMeasurement[0];
        m_LastMeasurement[0] = rssi;

        if (m_RandomWalkState == STATE_MEAS)
          m_RandomWalkState = m_NextRandomWalkState;

        // sample walk
        if (m_SampleWalkState == STATE_SAMPLE_MEAS)
        {
          m_SampleWalkMeasurements[m_SampleWalkSample] = rssi;
          m_SampleWalkSample++;
          m_SampleWalkState = m_NextSampleWalkState;
        }

        // ///////
        break;
      }

      default:
        break;
    }

    break;
  }

  default:
    break;
  }
}

RSSIClientData_t *ApplicationRSSIMeasServer::getClientData(uint8_t clientNum)
{
  std::list<RSSIClientData_t>::iterator it;
  for (it = m_ClientList.begin(); it != m_ClientList.end(); it++)
  {
    if (it->clientNum == clientNum)
    {
      return &(*it);
    }
  }
  return NULL;
}

RSSIClientData_t *ApplicationRSSIMeasServer::getClientData(uint8_t* mac)
{
  std::list<RSSIClientData_t>::iterator it;
  for (it = m_ClientList.begin(); it != m_ClientList.end(); it++)
  {
    // MAC equals iterators MAC
    bool unequal = false;
    for (uint8_t i = 0; i < 6; i++)
    {
      if (mac[i] != it->mac[i])
      {
        unequal = true;
        break;
      }
    }
    if (!unequal)
    {
      return &(*it);
    }
  }
  return NULL;
}

bool ApplicationRSSIMeasServer::printClientData(std::string &s)
{
  std::list<RSSIClientData_t>::iterator it;
  for (it = m_ClientList.begin(); it != m_ClientList.end(); it++)
  {
    logDebug("Client Num: %d\n", it->clientNum);
    logDebug("MAC: %d:%d:%d:%d:%d:%d\n", it->mac[0], it->mac[1], it->mac[2], it->mac[3], it->mac[4], it->mac[5]);
    std::list<RSSIMeasurement_t>::iterator it2;
    for (it2 = it->measurementList.begin(); it2 != it->measurementList.end(); it2++)
    {
      logDebug("%s / %d\n", it2->gondolaPos.toString().c_str(), it2->clientRSSI);
    }
  }
  return true;
}

void ApplicationRSSIMeasServer::initiateMeasurement()
{
  uint8_t data[1] = {RSSI_MEAS_S_MEAS_CMD};
  m_WebSocketServer.broadcastBIN(data, 1);
}

bool ApplicationRSSIMeasServer::appStartCommand(std::string &s)
{
  m_RandomWalkState = STATE_START;
  m_SampleWalkState = STATE_SAMPLE1;
  m_LastMeasurement[0] = m_LastMeasurement[1] = 0x80000000;
  return true;
}

bool ApplicationRSSIMeasServer::appStopCommand(std::string &s)
{
  m_RandomWalkState = STATE_INIT;
  m_SampleWalkState = STATE_SAMPLE_INIT;
  return true;
}

void ApplicationRSSIMeasServer::randomWalk(Gondola *gondola, float speed)
{
  switch(m_RandomWalkState)
  {
    case STATE_START:
    {
      if (!gondola->isIdle())
        break; // movement not finished

      logDebug("State init\n");
      Coordinate startPos = gondola->getCurrentPosition();
      // Go to defined height
      startPos.x = 40;
      gondola->setTargetPosition(startPos, speed);
      m_RandomWalkState = STATE_MOVE_AND_MEAS;
      // m_NextRandomWalkState = STATE_DIR_ZN;
      m_NextRandomWalkState = STATE_DIR_YN;
      break;
    }

    case STATE_DIR_ZN:
    {
      logDebug("State dir z- %d - %d\n", m_LastMeasurement[1], m_LastMeasurement[0]);
      // Value gets worse (more negative)
      if ((m_LastMeasurement[1] > m_LastMeasurement[0]) && (m_LastMeasurement[1] != (int32_t)0x80000000))
      {
        // rewert last step
        Coordinate newPos(gondola->getCurrentPosition());
        newPos.z += 20;
        gondola->setTargetPosition(newPos, speed);
        m_RandomWalkState = STATE_MOVE_AND_MEAS;
        m_NextRandomWalkState = STATE_DIR_ZP;
        m_LastMeasurement[0] = m_LastMeasurement[1] = 0x80000000;
        break;
      }
      Coordinate newPos(gondola->getCurrentPosition());
      newPos.z -= 20;
      gondola->setTargetPosition(newPos, speed);
      m_RandomWalkState = STATE_MOVE_AND_MEAS;
      m_NextRandomWalkState = STATE_DIR_ZN;
      break;
    }

    case STATE_DIR_ZP:
    {
      logDebug("State dir z+ %d - %d\n", m_LastMeasurement[1], m_LastMeasurement[0]);
      // Value gets worse (more negative)
      if (m_LastMeasurement[1] > m_LastMeasurement[0] && (m_LastMeasurement[1] != (int32_t)0x80000000))
      {
        // rewert step
        Coordinate newPos(gondola->getCurrentPosition());
        newPos.z -= 20;
        gondola->setTargetPosition(newPos, speed);
        m_RandomWalkState = STATE_MOVE_AND_MEAS;
        m_NextRandomWalkState = STATE_DIR_YN;
        m_LastMeasurement[0] = m_LastMeasurement[1] = 0x80000000;
        break;
      }
      Coordinate newPos(gondola->getCurrentPosition());
      newPos.z += 20;
      gondola->setTargetPosition(newPos, speed);
      m_RandomWalkState = STATE_MOVE_AND_MEAS;
      m_NextRandomWalkState = STATE_DIR_ZP;
      break;
    }

    case STATE_DIR_YN:
    {
      logDebug("State dir y- %d - %d\n", m_LastMeasurement[1], m_LastMeasurement[0]);
      // Value gets worse (more negative)
      if ((m_LastMeasurement[1] > m_LastMeasurement[0]) && (m_LastMeasurement[1] != (int32_t)0x80000000))
      {
        Coordinate newPos(gondola->getCurrentPosition());
        newPos.y += 10;
        gondola->setTargetPosition(newPos, speed);
        m_RandomWalkState = STATE_MOVE_AND_MEAS;
        m_NextRandomWalkState = STATE_DIR_YP;
        m_LastMeasurement[0] = m_LastMeasurement[1] = 0x80000000;
        break;
      }
      Coordinate newPos(gondola->getCurrentPosition());
      newPos.y -= 10;
      gondola->setTargetPosition(newPos, speed);
      m_RandomWalkState = STATE_MOVE_AND_MEAS;
      m_NextRandomWalkState = STATE_DIR_YN;
      break;
    }

    case STATE_DIR_YP:
    {
      logDebug("State dir y+ %d - %d\n", m_LastMeasurement[1], m_LastMeasurement[0]);
      // Value gets worse (more negative)
      if (m_LastMeasurement[1] > m_LastMeasurement[0] && (m_LastMeasurement[1] != (int32_t)0x80000000))
      {
        // rewert step
        Coordinate newPos(gondola->getCurrentPosition());
        newPos.y -= 10;
        gondola->setTargetPosition(newPos, speed);
        m_RandomWalkState = STATE_MOVE_AND_MEAS;
        m_NextRandomWalkState = STATE_WAIT;
        m_NextStart = millis() + 10000;
        m_LastMeasurement[0] = m_LastMeasurement[1] = 0x80000000;
        break;
      }
      Coordinate newPos(gondola->getCurrentPosition());
      newPos.y += 10;
      gondola->setTargetPosition(newPos, speed);
      m_RandomWalkState = STATE_MOVE_AND_MEAS;
      m_NextRandomWalkState = STATE_DIR_YP;
      break;
    }

    case STATE_MOVE_AND_MEAS:
    {
      if (!gondola->isIdle())
        break; // movement not finished

      m_RandomWalkState = STATE_MEAS;
      initiateMeasurement();
      break;
    }

    case STATE_WAIT:
        if (millis() > m_NextStart)
        {
          m_LastMeasurement[0] = m_LastMeasurement[1] = 0x80000000;
          m_RandomWalkState = STATE_MOVE_AND_MEAS;
          m_NextRandomWalkState = STATE_DIR_YN;
        }
      break;

    default:
      break;
  }
}

void ApplicationRSSIMeasServer::sampleWalk(Gondola *gondola, float speed)
{
  static Coordinate targetPos[3] = {{40, 50, 0}, {40, 150, 0}, {40, 250, 0}};
  static Coordinate secondGrid[3] = {{0, -25, 0}, {0, 0, 0}, {0, 25, 0}};
  static Coordinate bestCoord;

  switch(m_SampleWalkState)
  {
    case STATE_SAMPLE1:
      gondola->setTargetPosition(targetPos[m_SampleWalkSample], speed);

      m_SampleWalkState = STATE_SAMPLE_MOVE_AND_MEAS;
      if (m_SampleWalkSample == 2)
        m_NextSampleWalkState = STATE_SAMPLE_EVA_1;
      else
        m_NextSampleWalkState = STATE_SAMPLE1;
      break;

    case STATE_SAMPLE_EVA_1:
      {
        // calc best measurement
        int32_t min = std::max(m_SampleWalkMeasurements[0], m_SampleWalkMeasurements[1]);
        min = std::max(min, m_SampleWalkMeasurements[2]);

        // save index
        if (m_SampleWalkMeasurements[0] == min)
          bestCoord = targetPos[0];
        if (m_SampleWalkMeasurements[1] == min)
          bestCoord = targetPos[1];
        if (m_SampleWalkMeasurements[2] == min)
          bestCoord = targetPos[2];

        m_SampleWalkSample = 0;
        m_SampleWalkState = STATE_SAMPLE2;
        break;
      }

    case STATE_SAMPLE2:
      {
        Coordinate targetPos2 = bestCoord;
        targetPos2 = targetPos2 + secondGrid[m_SampleWalkSample];
        gondola->setTargetPosition(targetPos2, speed);

        m_SampleWalkState = STATE_SAMPLE_MOVE_AND_MEAS;
        if (m_SampleWalkSample == 2)
          m_NextSampleWalkState = STATE_SAMPLE_EVA_2;
        else
          m_NextSampleWalkState = STATE_SAMPLE2;
        break;
      }

    case STATE_SAMPLE_EVA_2:
      {
        // calc best measurement
        int32_t min = std::max(m_SampleWalkMeasurements[0], m_SampleWalkMeasurements[1]);
        min = std::max(min, m_SampleWalkMeasurements[2]);

        // save index
        if (m_SampleWalkMeasurements[0] == min)
          bestCoord = bestCoord + secondGrid[0];
        if (m_SampleWalkMeasurements[1] == min)
          bestCoord = bestCoord + secondGrid[1];
        if (m_SampleWalkMeasurements[2] == min)
          bestCoord = bestCoord + secondGrid[2];

        gondola->setTargetPosition(bestCoord, speed);

        m_SampleWalkSample = 0;
        m_SampleWalkState = STATE_SAMPLE_FIN;
        break;
      }

    case STATE_SAMPLE_MOVE_AND_MEAS:
      if (!gondola->isIdle())
        break; // movement not finished

      m_SampleWalkState = STATE_SAMPLE_MEAS;
      initiateMeasurement();
      break;

    case STATE_SAMPLE_FIN:
      break;

    default:
      break;
  }
}
