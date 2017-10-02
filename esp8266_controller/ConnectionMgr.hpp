#ifndef _CONNECTION_MGR_HPP_
#define _CONNECTION_MGR_HPP_

#include "IConnection.hpp"
#include "SerialConnection.hpp"
#include "WebServer.hpp"
#include "IWebSocket.hpp"

/**
 * Enumeration for all possible wireless connection types
 */
typedef enum connectionType_e : byte {
  CON_NONE,                             //!< No connection
  CON_ACCESS_POINT,                     //!< Open an access point
  CON_WIFI_CONNECTION,                  //!< Connect to a WiFi network
  CON_DUAL_CONNECTION                   //!< Open an access point and connect to a WiFi network
} connectionType_t;

/**
 * Enumeration for all possible WebSocket types
 */
typedef enum webSocketType_e : byte {
  WEBSOCKET_NONE,                            //!< Don't provide a WebSocket
  WEBSOCKET_SERVER,                          //!< Open a WebSocketServer
  WEBSOCKET_CLIENT,                          //!< Connect to a given WebSocketServer as a WebSocketClient
} webSocketType_t;

/**
 * Class to manage the network connection of the chip. Also server and client
 * connections are managed.
 * WebSocketServer or WebSocketClient
 * WebServer for user interaction
 */
class ConnectionMgr
{
public:
  /**
   * Get the instance of ConnectionMgr
   * @return pointer to instance
   */
  static ConnectionMgr *get();

  /**
   * Virtual destructor
   */
  virtual ~ConnectionMgr();

  /**
   * Change the type of connection
   * @param connetionType type of new connection
   */
  void changeConnection(connectionType_t connectionType);

  /**
  * Request a change of the connection. Usefull if change should
  * be done inside a WebServer etc.
  * to avoid conflicts when deleting the old connection.
  * Connection will be changed during the next loop() execution.
  * @param connectionType connection type to use
  */
  void requestChangeConnection(connectionType_t connectionType);

  /**
   * Get the current connection type
   * @return connection type
   */
  connectionType_t getConnectionType();

  /**
   * Change the type of the WebSocket
   * @param webSocketType type of new WebSocket
   */
  void changeWebSocket(webSocketType_t webSocketType);

  /**
   * Get the current used WebSocketType
   * @return current used WebSocketType
   */
  webSocketType_t getWebSocketType();

  /**
   * Get a reference to the web server
   * @return reference to web server
   */
  WebServer &getWebServer();

  /**
   * Call loop() frequently to handle change requests
   * -> Handle change of connection type
   */
  void loop();

  /**
   * Reset all connections
   */
  static void reset();

private:
  /**
   * Constructor
   */
  ConnectionMgr();

  /**
   * Serial command to change the connection
   * @param s serial input with all commandword and all arguments
   */
  bool contypeCommand(std::string &s);

  // Instance
  static ConnectionMgr         *s_Instance;                 //!< instance of singleton

  // Membervariables
  // Connection
  connectionType_t              m_ConnectionType;           //!< type of connection to provide
  connectionType_t              m_ChangeConnectionType;     //!< type of connection to change to
  bool                          m_ChangeConnectionRequest;  //!< indicates a request to change the connection
  IConnection                  *m_Connection;               //!< Pointer to current connection
  // WebSocket
  webSocketType_t               m_WebSocketType;            //!< Current used WebSocket tye
  IWebSocket                   *m_WebSocket;                //!< Pointer to WebSocket to provide
  // WebServer
  WebServer                     m_WebServer;                //!< Pointer to current WebServer
};

#endif /* _CONNECTION_MGR_HPP_ */
