#ifndef _APPLICATION_RSSI_MEAS_SERVER_HPP_
#define _APPLICATION_RSSI_MEAS_SERVER_HPP_

#include <WebSocketsServer.h>
#include <list>
#include "Coordinate.hpp"
#include "ESP8266WebServer.h"
#include "Gondola.hpp"

/**
 * Grid siz for the sample walk
 */
#define SAMPLE_WALK_GRID_SIZE     5

/**
 * Enumeration for web socket commands
 */
typedef enum : byte {
  RSSI_MEAS_C_REG,      //!< client registers at server
  RSSI_MEAS_C_MEAS,     //!< client sends measurement to server
  RSSI_MEAS_S_MEAS_CMD  //!< server orders measurement
} ApplicationRSSIMeasCmd_t;

/**
 * Strucsture for a measurement
 */
typedef struct {
  Coordinate gondolaPos;    //!< current position
  int32_t clientRSSI;       //!< measured RSSI
} RSSIMeasurement_t;

/**
 * Strucsture for an application client
 */
typedef struct {
  uint8_t clientNum;        //!<< web socket number
  uint8_t mac[6];           //!< mac address for authentification
  IPAddress clientIP;       //!< ip address of client
  std::list<RSSIMeasurement_t> measurementList;   //!< list of measurements
  bool connected;           //!< currently connected
} RSSIClientData_t;

/**
 * Enumeration for states in random walk algorithm
 */
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

/**
 * Enumeration for state in sample walk algorithm
 */
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

/**
 * Class for the server of the RSSI Measurement application
 */
class ApplicationRSSIMeasServer
{
public:
  /**
   * Construcotr
   * @param port port which has to be used for the web socket server
   */
  ApplicationRSSIMeasServer(uint16_t port);

  /**
   * virtual destructor
   */
  virtual ~ApplicationRSSIMeasServer();

  /**
   * Loop to call in systems main loop for the web socket connection
   */
  void loop();

private:

  /**
   * Event from web ocket implementation
   * @param num     number of client, that sends the message
   * @param type    type of the message
   * @param payload playload of the message
   * @param length  length of the message
   */
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

  /**
   * Get a clients data by its number
   * @param  clientNum clients number
   * @return           data of client
   */
  RSSIClientData_t *getClientData(uint8_t clientNum);

  /**
   * Get a clients data by its mac address
   * @param  mac clients mac address
   * @return     data of client
   */
  RSSIClientData_t *getClientData(uint8_t* mac);

  /**
   * CLI Command to print all measurements
   * @param  s command line string
   * @return   success
   */
  bool appPrintCommand(std::string &s);

  /**
   * initiade measurements at all clients
   */
  void initiateMeasurement();

  /**
   * CLI Command to start the application
   * @param  s command line string
   * @return   success
   */
  bool appStartCommand(std::string &s);

  /**
   * CLI command to stop the application
   * @param  s command line s
   * @return   success
   */
  bool appStopCommand(std::string &s);

  /**
   * Execute next random walk step
   * @param gondola pointer to the used Gondola
   * @param speed   speed to use for movements
   */
  void randomWalk(Gondola *gondola, float speed);

  /**
   * Execute next sample walk step
   * @param gondola pointer to the used Gondola
   * @param speed   speed to use for movements
   */
  void sampleWalk(Gondola *gondola, float speed);

  uint16_t                        m_Port;                 //!< Port for web socket
  WebSocketsServer                m_WebSocketServer;      //!< Server from WebSocketsLibrary
  std::list<RSSIClientData_t>     m_ClientList;           //!< List of all clients
  uint32_t                        m_NextPing;             //!< Next ping for timeout detection
  uint32_t                        m_NextStart;            //!< next start time if algorithm supports automatic restart

  // Random Walk
  int32_t                         m_LastMeasurement[2];   //!< Last 2 measurements
  rssiMeasRandomWalkState_t       m_RandomWalkState;      //!< state of random walk
  rssiMeasRandomWalkState_t       m_NextRandomWalkState;  //!< next state for random walk after measurement

  // SampleWalk
  int32_t                         m_SampleWalkMeasurements[SAMPLE_WALK_GRID_SIZE];  //!< Measurements of sample walk
  uint8_t                         m_SampleWalkSample;   //!< current index in m_SampleWalkMeasurements
  rssiMeasSampleWalkState_t       m_SampleWalkState;    //!< state of sample walk algorithm
  rssiMeasSampleWalkState_t       m_NextSampleWalkState;  //!< next state for sample walk after measurement

};

#endif /* _APPLICATION_RSSI_MEAS_SERVER_HPP_ */
