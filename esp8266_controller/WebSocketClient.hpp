#ifndef _WEB_SOCKET_CLIENT_HPP_
#define _WEB_SOCKET_CLIENT_HPP_

#include <Arduino.h>
#include <WebSocketsClient.h>
#include "HardwareAnchor.hpp"
#include "IWebSocket.hpp"

/**
 * Client of a WebSocket
 */
class WebSocketClient : public IWebSocket
{
public:
  /**
   * Constructor
   * @param host Host to connect to
   * @param port port at host
   */
  WebSocketClient(std::string host, uint16_t port);

  /**
   * Virtual destructor
   */
  virtual ~WebSocketClient();

  /**
   * Loop to call frequently
   */
  virtual void loop();

private:
  /**
   * Event appears, when WebSockets reveives a message
   * @param type    Type of message
   * @param payload message content
   * @param length  length of payload
   */
  void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

  /**
   * Is registered at anchor to inform server about the finished movement
   */
  bool anchorReadyCallback();

  // membervariables
  std::string             m_Host;                     //!< WebSocket host
  uint16_t                m_Port;                     //!< WebSocket port
  WebSocketsClient        m_WebSocketClient;          //!< Client instance of WebSocketsLibrary
  HardwareAnchor          m_Anchor;                   //!< Hardware anchor
};

#endif /* _WEB_SOCKET_CLIENT_HPP_ */
