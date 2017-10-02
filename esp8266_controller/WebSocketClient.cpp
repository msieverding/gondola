#include "WebSocketClient.hpp"
#include "ConnectionMgr.hpp"
#include <functional>
#include "Log.hpp"

WebSocketClient::WebSocketClient(std::string host, uint16_t port)
 : m_Host(host)
 , m_Port(port)
 , m_WebSocketClient()
 , m_Anchor(0)    // NO ID necessary in client
{
  logDebug("Start WebSocketClient and connect to '%s:%d'\n", m_Host.c_str(), m_Port);
#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
  logDebug("WebSocketClient runs in asynchronous mode!\n");
#endif
  // server address, port and URL
	m_WebSocketClient.begin(m_Host.c_str(), m_Port, "/");

	// event handler
	m_WebSocketClient.onEvent(std::bind(&WebSocketClient::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// // use HTTP Basic Authorization this is optional remove if not needed
	// m_WebSocketClient.setAuthorization("user", "Password");

#if (WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
	// try every 30s again if connection has failed
  m_WebSocketClient.setReconnectInterval(30000);
#endif

  m_Anchor.registerReadyCallback(std::bind(&WebSocketClient::anchorReadyCallback, this));
}

WebSocketClient::~WebSocketClient()
{

}

void WebSocketClient::loop()
{
#if (WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
  m_WebSocketClient.loop();
#endif
  m_Anchor.loop();
}

void WebSocketClient::webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
  switch(type)
  {
		case WStype_DISCONNECTED:
			logDebug("[WSc] Disconnected!\n");
			break;

		case WStype_CONNECTED:
    {
			logDebug("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			m_WebSocketClient.sendTXT("Connected");

      byte msg[16 + 1];
      uint8_t i = 1;
      b4Converter_t converter;

      msg[0] = WSO_C_REGISTER;

      converter.f = m_Anchor.getAnchorPos().x;
      msg[i++] = converter.b[0];
      msg[i++] = converter.b[1];
      msg[i++] = converter.b[2];
      msg[i++] = converter.b[3];

      converter.f = m_Anchor.getAnchorPos().y;
      msg[i++] = converter.b[0];
      msg[i++] = converter.b[1];
      msg[i++] = converter.b[2];
      msg[i++] = converter.b[3];

      converter.f = m_Anchor.getAnchorPos().z;
      msg[i++] = converter.b[0];
      msg[i++] = converter.b[1];
      msg[i++] = converter.b[2];
      msg[i++] = converter.b[3];

      converter.f = m_Anchor.getRopeOffset();
      msg[i++] = converter.b[0];
      msg[i++] = converter.b[1];
      msg[i++] = converter.b[2];
      msg[i++] = converter.b[3];

      m_WebSocketClient.sendBIN(msg, i);
      break;
		}

		case WStype_TEXT:
			logDebug("[WSc] get text: %s\n", payload);
			break;

		case WStype_BIN:
    {
			logDebug("[WSc] get binary length: %u\n", length);

      webSocketCommand_t cmd = static_cast<webSocketCommand_t>(payload[0]);

      if (cmd == WSO_S_SPOOLED_DIST)
      {
        b4Converter_t converter;
        Coordinate goPos;
        uint8_t i = 1;
        converter.b[0] = payload[i++];
        converter.b[1] = payload[i++];
        converter.b[2] = payload[i++];
        converter.b[3] = payload[i++];

        m_Anchor.setInitialSpooledDistance(converter.f);
      }
      else if (cmd == WSO_S_MOVE)
      {
        b4Converter_t newSpooledDistance;
        b4Converter_t travelTime;
        uint8_t i = 1;

        newSpooledDistance.b[0] = payload[i++];
        newSpooledDistance.b[1] = payload[i++];
        newSpooledDistance.b[2] = payload[i++];
        newSpooledDistance.b[3] = payload[i++];

        travelTime.b[0] = payload[i++];
        travelTime.b[1] = payload[i++];
        travelTime.b[2] = payload[i++];
        travelTime.b[3] = payload[i++];

        logDebug("Got new spooled distance = '%s' and travel time = '%u'\n", floatToString(newSpooledDistance.f).c_str(), travelTime.u);
        m_Anchor.setTargetSpooledDistance(newSpooledDistance.f);
        m_Anchor.startMovement(travelTime.u);
      }
			break;
    }

    default:
      break;
  }
}

bool WebSocketClient::anchorReadyCallback()
{
  uint8_t payload = WSO_C_REPORT;
  return m_WebSocketClient.sendBIN(&payload, 1);
}
