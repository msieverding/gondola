#ifndef _APPLICATION_RSSI_MEAS_SERVER_HPP_
#define _APPLICATION_RSSI_MEAS_SERVER_HPP_

#include <WebSocketsServer.h>
#include <list>
#include "Coordinate.hpp"
#include "ESP8266WebServer.h"
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

typedef enum rssiMeasState_e : byte {
  STATE_CONNECT,
  STATE_INIT,
  STATE_DIR_ZN,
  STATE_DIR_ZP,
  STATE_DIR_YN,
  STATE_DIR_YP,
  STATE_FIN,
  STATE_MOVE_AND_MEAS,
  STATE_MEAS
} rssiMeasState_t;

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
  void initiateMeasurement();

  uint16_t                        m_Port;
  WebSocketsServer                m_WebSocketServer;  //!< Server from WebSocketsLibrary
  std::list<RSSIClientData_t>     m_ClientList;
  uint32_t                        m_NextPing;
  int32_t                         m_LastMeasurement[2];
  rssiMeasState_t                 m_State;
  rssiMeasState_t                 m_NextState;
};

#endif /* _APPLICATION_RSSI_MEAS_SERVER_HPP_ */
