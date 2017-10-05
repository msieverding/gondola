#include "ApplicationRSSIMeasClient.hpp"
#include "ApplicationRSSIMeasServer.hpp"
#include "Log.hpp"
#include <functional>

ApplicationRSSIMeasClient::ApplicationRSSIMeasClient(std::string host, uint16_t port)
 : m_Host(host)
 , m_Port(port)
 , m_WebSocketClient()
 , m_NextMeasTime(0)
{
  logDebug("Start Application RSSIMeasClient with webSocket on host:port %s:%d\n", m_Host.c_str(), m_Port);
#if (WEBSOCKETS_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)
  logDebug("WebSocketClient runs in asynchronous mode!\n");
#endif
  // server address, port and URL
  m_WebSocketClient.begin(m_Host.c_str(), m_Port, "/");

  // event handler
  m_WebSocketClient.onEvent(std::bind(&ApplicationRSSIMeasClient::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

  // // use HTTP Basic Authorization this is optional remove if not needed
  // m_WebSocketClient.setAuthorization("user", "Password");

#if (WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
  // try every 30s again if connection has failed
  m_WebSocketClient.setReconnectInterval(30000);
#endif
}

ApplicationRSSIMeasClient::~ApplicationRSSIMeasClient()
{

}

void ApplicationRSSIMeasClient::loop()
{
  // if (millis() > m_NextMeasTime)
  // {
  //   m_NextMeasTime = millis() + 2000;
  //
  //   b4Converter_t rssi;
  //   rssi.i = WiFi.RSSI();
  //   uint8_t data[5];
  //   data[0] = RSSI_MEAS_C_MEAS;
  //   data[1] = rssi.b[0];
  //   data[2] = rssi.b[1];
  //   data[3] = rssi.b[2];
  //   data[4] = rssi.b[3];
  //   m_WebSocketClient.sendBIN(data, 5);
  // }

#if (WEBSOCKETS_NETWORK_TYPE != NETWORK_ESP8266_ASYNC)
  m_WebSocketClient.loop();
#endif
}

void ApplicationRSSIMeasClient::webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
  switch(type)
  {
		case WStype_DISCONNECTED:
			logDebug("[WSc] Disconnected!\n");
			break;

		case WStype_CONNECTED:
			logDebug("[WSc] Connected to url: %s\n", payload);
      // Register
      uint8_t data[7];
      data[0] = RSSI_MEAS_C_REG;
      WiFi.macAddress(data + 1);
      m_WebSocketClient.sendBIN(data, 7);
			// send message to server when Connected
			m_WebSocketClient.sendTXT("Connected");
      break;

		case WStype_TEXT:
			logDebug("[WSc] get text: %s\n", payload);
			break;

		case WStype_BIN:
    {
      ApplicationRSSIMeasCmd_t cmd = static_cast<ApplicationRSSIMeasCmd_t>(payload[0]);
      switch (cmd)
      {
        case RSSI_MEAS_S_MEAS_CMD:
        {
          b4Converter_t rssi;
          rssi.i = WiFi.RSSI();
          uint8_t data[5];
          data[0] = RSSI_MEAS_C_MEAS;
          data[1] = rssi.b[0];
          data[2] = rssi.b[1];
          data[3] = rssi.b[2];
          data[4] = rssi.b[3];
          m_WebSocketClient.sendBIN(data, 5);
          break;
        }

        default:
          break;
      }
			logDebug("[WSc] get binary length: %u\n", length);
			break;
    }

    default:
      break;
  }
}
