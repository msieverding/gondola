#ifndef _APPLICATION_RSSI_MEAS_CLIENT_HPP_
#define _APPLICATION_RSSI_MEAS_CLIENT_HPP_

#include "WebSocketsClient.h"


/**
 * Client application for the RSSI Measurement Application
 */
class ApplicationRSSIMeasClient
{
public:
  /**
   * Constructor
   * @param host host of application server
   * @param port application port  on host for
   */
  ApplicationRSSIMeasClient(std::string host, uint16_t port);

  /**
   * Virtual destructor
   */
  virtual ~ApplicationRSSIMeasClient();

  /**
   * Loop to execute in main system loop to handle the WebSocket fthis class
   */
  void loop();

private:
  /**
   * Callback for an WebSocket event
   * @param type    type of the message
   * @param payload payload of the message
   * @param length  length of the payload
   */
  void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

  // membervariables
  std::string             m_Host;                     //!< WebSocket host
  uint16_t                m_Port;                     //!< WebSocket port
  WebSocketsClient        m_WebSocketClient;          //!< Client instance of WebSocketsLibrary
  uint32_t                m_NextMeasTime;             //!< time for next measurement
  int32_t                 m_Measurements[20];         //!< Buffer for 20 m_Measurements
  uint8_t                 m_Measurement;              //!< current index in m_Measurements
};

#endif /* _APPLICATION_RSSI_MEAS_CLIENT_HPP_ */
