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
 , m_NextMeas(millis() + MEASUREMENT_INTERVALL)
 , m_MovementList()
{
  logDebug("Start Application RSSIMeasServer with webSocket on port %d\n", m_Port);
#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
  logDebug("WebSocketServer runs in asynchronous mode!\n");
#endif
  m_WebSocketServer.begin();
  m_WebSocketServer.onEvent(std::bind(&ApplicationRSSIMeasServer::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  CommandInterpreter::get()->addCommand("appPrint", std::bind(&ApplicationRSSIMeasServer::printClientData, this, std::placeholders::_1));
}

ApplicationRSSIMeasServer::~ApplicationRSSIMeasServer()
{
  CommandInterpreter::get()->deleteCommand("appPrint");
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

  if (millis() > m_NextMeas)
  {
    uint8_t data[1] = {RSSI_MEAS_S_MEAS_CMD};
    m_WebSocketServer.broadcastBIN(data, 1);
    m_NextMeas += MEASUREMENT_INTERVALL;
  }

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
        // TODO handle measurement
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
    logDebug("MAC_in:   %d:%d:%d:%d:%d:%d\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    logDebug("MAC_list: %d:%d:%d:%d:%d:%d\n", it->mac[0], it->mac[1], it->mac[2], it->mac[3], it->mac[4], it->mac[5]);
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
      logDebug("Equal\n");
      return &(*it);
    }
  }
  logDebug("Unequal\n");
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
