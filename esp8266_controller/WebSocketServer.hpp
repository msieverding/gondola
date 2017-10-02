#ifndef _WEB_SOCKET_SERVER_HPP_
#define _WEB_SOCKET_SERVER_HPP_

#include <Arduino.h>
#include <Hash.h>   // Need to include it here, since WebSocketsServer library will not find it... That would result in a build error
#include <WebSocketsServer.h>
#include <list>
#include "Coordinate.hpp"
#include "Log.hpp"
#include "Gondola.hpp"
#include "IWebSocket.hpp"
#include "HardwareAnchor.hpp"
/**
 * WebSocket Server as Server for gondola
 */
class WebSocketServer : public IWebSocket
{
public:
  /**
   * Constructor
   * @param port port to use
   */
  WebSocketServer(uint16_t port);

  /**
   * Virtual destructor
   */
  virtual ~WebSocketServer();

  /**
   * Loop to call frequently
   */
  virtual void loop();

  /**
   * Get the gondola of this server
   * @return pointer to gondola
   */
  Gondola *getGondola();

private:
  /**
   * Event that is trigered on incomming message
   * @param num     Client number
   * @param type    type of data
   * @param payload message content
   * @param length  length of payload
   */
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

  /**
   * Function to call, when an anchor should move.
   * Is registered at gonola and called from there
   * @param anchorInfo information about the anchor
   * @return           success
   */
  bool remoteAnchorMoveFunction(IAnchor *anchor);

  /**
   * Function to call, when an anchor should be initialised.
   * Is registered at gonola and called from there
   * @param anchorInfo information about the anchor
   * @return           success
   */
  bool remoteAnchorInitFunction(IAnchor *anchor);

  /**
   * Send gondola, that an anchor finished the movement
   * @param  anchor anchor that finished
   * @return        ever true
   */
  bool readyCallbackToGondola(IAnchor *anchor);

  // membervariables
  uint16_t                        m_Port;             //!< Port to use
  WebSocketsServer                m_WebSocketServer;  //!< Server from WebSocketsLibrary
  uint32_t                        m_NextPing;         //!< Time for next ping
  HardwareAnchor                  m_Anchor;           //!< HardwareAnchor of this board
  Gondola                         m_Gondola;          //!< pointer to Gondola
};

#endif /* _WEB_SOCKET_SERVER_HPP_ */
