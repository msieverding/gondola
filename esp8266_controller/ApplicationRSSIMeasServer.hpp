#ifndef _APPLICATION_RSSI_MEAS_SERVER_HPP_
#define _APPLICATION_RSSI_MEAS_SERVER_HPP_

#include <WebSocketsServer.h>
#include <list>
#include "Coordinate.hpp"
#include "ESP8266WebServer.h"
#include "Gondola.hpp"
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

typedef enum rssiMeasRandomWalkState_e : byte {
  STATE_INIT,
  STATE_START,
  STATE_DIR_ZN,
  STATE_DIR_ZP,
  STATE_DIR_YN,
  STATE_DIR_YP,
  STATE_WAIT,
  STATE_MOVE_AND_MEAS,
  STATE_MEAS
} rssiMeasRandomWalkState_t;

typedef enum rssiMeasSampleWalkState_e : byte {
  STATE_SAMPLE_INIT,
  STATE_SAMPLE1,
  STATE_SAMPLE_EVA_1,
  STATE_SAMPLE2,
  STATE_SAMPLE_EVA_2,
  STATE_SAMPLE_MOVE_AND_MEAS,
  STATE_SAMPLE_MEAS,
  STATE_SAMPLE_FIN
} rssiMeasSampleWalkState_t;

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
  bool appStartCommand(std::string &s);
  bool appStopCommand(std::string &s);
  void randomWalk(Gondola *gondola, float speed);
  void sampleWalk(Gondola *gondola, float speed);

  uint16_t                        m_Port;
  WebSocketsServer                m_WebSocketServer;  //!< Server from WebSocketsLibrary
  std::list<RSSIClientData_t>     m_ClientList;
  uint32_t                        m_NextPing;
  uint32_t                        m_NextStart;

  // Random Walk
  int32_t                         m_LastMeasurement[2];
  rssiMeasRandomWalkState_t       m_RandomWalkState;
  rssiMeasRandomWalkState_t       m_NextRandomWalkState;

  // SampleWalk
  int32_t                         m_SampleWalkMeasurements[3];
  uint8_t                         m_SampleWalkSample;
  rssiMeasSampleWalkState_t       m_SampleWalkState;
  rssiMeasSampleWalkState_t       m_NextSampleWalkState;
  uint8_t                         m_SampleWalkMax[2];

};

#endif /* _APPLICATION_RSSI_MEAS_SERVER_HPP_ */
