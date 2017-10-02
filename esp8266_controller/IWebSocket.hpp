#ifndef _I_WEB_SOCKET_HPP_
#define _I_WEB_SOCKET_HPP_

/**
 * Enumeration for commands that are used in the WebSocket communication
 * Prefix WSO_C is initiaded by Client
 * Prefix WSO_S is initiaded by Server
 */
typedef enum webSocketCommand_e : byte {
  WSO_C_REGISTER,       //!< Client want to register at server
  WSO_C_REPORT,         //!< Client reports, that spooling is finished
  WSO_S_SPOOLED_DIST,   //!< Server tells client, how much it is spooled
  WSO_S_MOVE            //!< Server wants, that client spooles
} webSocketCommand_t;

/**
 * Interface class for the WebSocket types
 */
class IWebSocket
{
public:
  /**
   * Empty cunstructor
   */
  IWebSocket() {}

  /**
   * Virtual empty destructor
   */
  virtual ~IWebSocket() {}

  /**
   * Pure virtual loop function
   */
  virtual void loop() = 0;

protected:

};

#endif /* _I_WEB_SOCKET_HPP_ */
