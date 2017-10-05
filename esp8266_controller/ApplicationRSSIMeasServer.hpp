#ifndef _APPLICATION_RSSI_MEAS_SERVER_HPP_
#define _APPLICATION_RSSI_MEAS_SERVER_HPP_

#include <WebSocketsServer.h>
#include <list>
#include "Coordinate.hpp"

// TODO Doc

typedef enum : byte {
  RSSI_MEAS_C_REG,
  RSSI_MEAS_C_MEAS,
  RSSI_MEAS_S_MEAS_CMD
} ApplicationRSSIMeasCmd_t;

typedef struct {
  Coordinate gondolaPos;
  int32_t clientRSSI;
} RSSIMeasurement_t;

typedef struct {
  uint8_t clientNum;
  uint8_t mac[6];
  IPAddress clientIP;
  std::list<RSSIMeasurement_t> measurementList;
  bool connected;
} RSSIClientData_t;

class ApplicationRSSIMeasServer
{
public:
  ApplicationRSSIMeasServer(uint16_t port);
  ~ApplicationRSSIMeasServer();
  void loop();

private:

  void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
  RSSIClientData_t *getClientData(uint8_t clientNum);
  RSSIClientData_t *getClientData(uint8_t* mac);
  bool printClientData(std::string &s);

  uint16_t                        m_Port;
  WebSocketsServer                m_WebSocketServer;  //!< Server from WebSocketsLibrary
  std::list<RSSIClientData_t>     m_ClientList;
  uint32_t                        m_NextPing;
  uint32_t                        m_NextMeas;
};

#endif /* _APPLICATION_RSSI_MEAS_SERVER_HPP_ */
