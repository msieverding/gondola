#include "WebSocketServer.hpp"
#include <functional>
#include "Log.hpp"
#include "RemoteAnchor.hpp"
#include "HardwareAnchor.hpp"
#include "ConnectionMgr.hpp"

WebSocketServer::WebSocketServer(uint16_t port)
 : m_Port(port)
 , m_WebSocketServer(m_Port)
 , m_NextPing(millis())
 , m_Anchor(HW_ANCHOR_ID)
 , m_Gondola()
{
  logDebug("Started WebSocketServer on port '%d'\n", m_Port);
#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
  logDebug("WebSocketServer runs in asynchronous mode!\n");
#endif
  m_WebSocketServer.begin();
  m_WebSocketServer.onEvent(std::bind(&WebSocketServer::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  m_Anchor.registerReadyCallback(std::bind(&WebSocketServer::readyCallbackToGondola, this, std::placeholders::_1));
  m_Gondola.addAnchor(&m_Anchor);

  ConnectionMgr::get()->getWebServer().registerGondola(&m_Gondola);
}

WebSocketServer::~WebSocketServer()
{
  ConnectionMgr::get()->getWebServer().registerGondola(NULL);
}

void WebSocketServer::loop()
{
  // Ping all clients to detect timeouts. See issue: https://github.com/Links2004/arduinoWebSockets/issues/203
  if (millis() > m_NextPing)
  {
    m_NextPing += 1000;
    std::list<IAnchor *> anchorList = m_Gondola.getAnchorList();
    std::list<IAnchor *>::iterator it = anchorList.begin();
    while (it != anchorList.end())
    {
      IAnchor *anchor = *it;
      if (anchor->getID() != HW_ANCHOR_ID)
      {
        if (m_WebSocketServer.sendPing(anchor->getID()) == false)
        {
          logWarning("Detected connection loss to client %d. Force disconnect.\n", anchor->getID());
          it++;   // increment here, because current it wil be deleted during disconnect
          m_WebSocketServer.disconnect(anchor->getID());
#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
          // Manual fired disconnect event is needed in async mode
          webSocketEvent(anchor->getID(), WStype_DISCONNECTED, NULL, 0);
#endif
          continue;
        }
      }
      it++;
    }
  }

#if (WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
  m_WebSocketServer.loop();
#endif
  m_Anchor.loop();
}

Gondola *WebSocketServer::getGondola()
{
  return &m_Gondola;
}

void WebSocketServer::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
 switch(type)
 {
  case WStype_DISCONNECTED:
    logDebug("[%u] Disconnected!\n", num);
    m_Gondola.deleteAnchor(num);
    break;

  case WStype_CONNECTED:
    {
      IPAddress ip = m_WebSocketServer.remoteIP(num);
      logDebug("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
  		// send message to client
  		m_WebSocketServer.sendTXT(num, "Connected");
    }
    break;

  case WStype_TEXT:
    logDebug("[%u] get Text: %s\n", num, payload);
    break;

  case WStype_BIN:
  {
    logDebug("[%u] get binary length: %u\n", num, length);

    webSocketCommand_t cmd = static_cast<webSocketCommand_t>(payload[0]);
    if (cmd == WSO_C_REGISTER)
    {
      RemoteAnchor *anchor = new RemoteAnchor(num);
      Coordinate anchorPos;
      b4Converter_t converter;
      uint8_t i = 1;
      converter.b[0] = payload[i++];
      converter.b[1] = payload[i++];
      converter.b[2] = payload[i++];
      converter.b[3] = payload[i++];
      anchorPos.x = converter.f;

      converter.b[0] = payload[i++];
      converter.b[1] = payload[i++];
      converter.b[2] = payload[i++];
      converter.b[3] = payload[i++];
      anchorPos.y = converter.f;

      converter.b[0] = payload[i++];
      converter.b[1] = payload[i++];
      converter.b[2] = payload[i++];
      converter.b[3] = payload[i++];
      anchorPos.z = converter.f;
      anchor->setAnchorPos(anchorPos);

      converter.b[0] = payload[i++];
      converter.b[1] = payload[i++];
      converter.b[2] = payload[i++];
      converter.b[3] = payload[i++];
      anchor->setRopeOffset(converter.f);

      anchor->registerInitCallback(std::bind(&WebSocketServer::remoteAnchorInitFunction, this, std::placeholders::_1));
      anchor->registerMoveCallback(std::bind(&WebSocketServer::remoteAnchorMoveFunction, this, std::placeholders::_1));
      anchor->registerReadyCallback(std::bind(&WebSocketServer::readyCallbackToGondola, this, std::placeholders::_1));
      m_Gondola.addAnchor(anchor);

      logDebug("Client registered at position(%s), ropeOffset %s\n", anchor->getAnchorPos().toString().c_str(), FTOS(anchor->getRopeOffset()));
    }
    else if (cmd == WSO_C_REPORT)
    {
      IAnchor *anchor = m_Gondola.getAnchor(num);
      anchor->executeReadyCallback();
    }
    break;
  }

  default:
    break;
  }
}

bool WebSocketServer::remoteAnchorMoveFunction(IAnchor *anchor)
{
  uint8_t i = 1;
  uint8_t payload[8 + 1];

  payload[0] = WSO_S_MOVE;
  b4Converter_t converter;

  converter.f = anchor->getTargetSpooledDistance();
  payload[i++] = converter.b[0];
  payload[i++] = converter.b[1];
  payload[i++] = converter.b[2];
  payload[i++] = converter.b[3];

  converter.u = anchor->getTravelTime();
  payload[i++] = converter.b[0];
  payload[i++] = converter.b[1];
  payload[i++] = converter.b[2];
  payload[i++] = converter.b[3];

  return m_WebSocketServer.sendBIN(anchor->getID(), payload, i);
}

bool WebSocketServer::remoteAnchorInitFunction(IAnchor *anchor)
{
  uint8_t i = 1;
  uint8_t payload[4 + 1];

  payload[0] = WSO_S_SPOOLED_DIST;
  b4Converter_t converter;

  converter.f = anchor->getSpooledDistance();
  payload[i++] = converter.b[0];
  payload[i++] = converter.b[1];
  payload[i++] = converter.b[2];
  payload[i++] = converter.b[3];

  return m_WebSocketServer.sendBIN(anchor->getID(), payload, i);
}

bool WebSocketServer::readyCallbackToGondola(IAnchor *anchor)
{
  logDebug("readyCallbackToGondola\n");
  m_Gondola.reportAnchorFinished(anchor->getID());
  return true;
}
