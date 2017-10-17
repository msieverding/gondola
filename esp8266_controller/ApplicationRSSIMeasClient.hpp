#ifndef _APPLICATION_RSSI_MEAS_CLIENT_HPP_
#define _APPLICATION_RSSI_MEAS_CLIENT_HPP_

#include "WebSocketsClient.h"

// TODO Doc

class ApplicationRSSIMeasClient
{
public:
  ApplicationRSSIMeasClient(std::string host, uint16_t port);
  ~ApplicationRSSIMeasClient();
  void loop();

private:
  void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

  // membervariables
  std::string             m_Host;                     //!< WebSocket host
  uint16_t                m_Port;                     //!< WebSocket port
  WebSocketsClient        m_WebSocketClient;          //!< Client instance of WebSocketsLibrary
  uint32_t                m_NextMeasTime;
  int32_t                 m_Measurements[20];
  uint8_t                 m_Measurement;
};

#endif /* _APPLICATION_RSSI_MEAS_CLIENT_HPP_ */
