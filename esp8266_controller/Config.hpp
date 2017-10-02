#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <string>
#include "Coordinate.hpp"
#include <IPAddress.h>
#include "Log.hpp"

// reduced in precision (0.05 cm = MIN_PRECISION = 1step = 1.8', 1 cm =
// 20steps = 36', 10 cm = 200steps = 360')
// the difference between the old distance and the new one is then
#define STEP_CM        20

// precision of 1 step in cm
#define MIN_PRECISION ((float)(1 / ((float)STEP_CM)))
#define MICROSTEPS 1

//! Forward declaration of connectionType_t from ConnectionMgr
enum connectionType_e : byte;
typedef enum connectionType_e connectionType_t;
//! Forward declaration of webSocketType_t from ConnectionMgr
enum webSocketType_e : byte;
typedef enum webSocketType_e webSocketType_t;

/**
 * Singleton class to store the setup of the application
 */
class Config
{
public:
  /**
   * get configuration instance
   * @return pointer to instance
   */
  static Config *get();

  /**
   * virtual destructor
   */
  virtual ~Config();

  /**
   * save the setup to the internal EEPROM
   * @return success
   */
  bool writeToEEPROM();

  /**
   * write the WC section to eeprom
   * @param persist (optional) Doesn't commit to EEPROM when false.
   * @see writeToEEPROM()
   */
  bool writeWCToEEPROM(bool persist = true);

  /**
   * write the AP section to eeprom
   * @param persist (optional) Doesn't commit to EEPROM when false.
   * @see writeToEEPROM()
   */
  bool writeAPToEEPROM(bool persist = true);

  /**
   * write the GO section to eeprom
   * @param persist (optional) Doesn't commit to EEPROM when false.
   * @see writeToEEPROM()
   */
  bool writeGOToEEPROM(bool persist = true);

  /**
   * read the setup from the internal EEPROM
   *
   * When checksum is invalid, the standart config is loaded
   */
  void readFromEEPROM();

  /**
   * load the default config. Use writeToEEPROM to store it into the EEPROM after loading it
   */
  static void resetConfig();

  /**
   * Print the configuration if log level is at least LOG_DEBUG
   */
  void printConfig(void);

private:
  /**
   * private constrcutor
   */
  Config();

public:
  // WiFi Connection
  std::string getWC_SSID() { return WC_SSID; }
  std::string getWC_PASSPHRASE() { return WC_PASSPHRASE; }
  std::string getWC_HOSTNAME() { return WC_HOSTNAME; }
  IPAddress getWC_IPADDRESS() { return WC_IPADDRESS; }
  IPAddress getWC_GATEWAY() { return WC_GATEWAY; }
  IPAddress getWC_NETMASK() { return WC_NETMASK; }

  void setWC_SSID(std::string ssid);
  void setWC_PASSPHRASE(std::string passphrase);
  void setWC_HOSTNAME(std::string name);
  void setWC_IPADDRESS(IPAddress ip);
  void setWC_IPADDRESS(String ip);
  void setWC_GATEWAY(IPAddress gw);
  void setWC_GATEWAY(String gw);
  void setWC_NETMASK(IPAddress nm);
  void setWC_NETMASK(String nm);

  // Access Point
  IPAddress getAP_IPADDRESS() { return AP_IPADDRESS; }
  IPAddress getAP_GATEWAY() { return AP_GATEWAY; }
  IPAddress getAP_NETMASK() { return AP_NETMASK; }
  std::string getAP_SSID() { return AP_SSID; }
  std::string getAP_PASSPHRASE() { return AP_PASSPHRASE; }
  std::string getAP_URL() { return AP_URL; }

  void setAP_IPADDRESS(IPAddress ip);
  void setAP_IPADDRESS(String ip);
  void setAP_GATEWAY(IPAddress gw);
  void setAP_GATEWAY(String gw);
  void setAP_NETMASK(IPAddress nm);
  void setAP_NETMASK(String nm);
  void setAP_SSID(std::string ssid);
  void setAP_PASSPHRASE(std::string passphrase);
  void setAP_URL(std::string url);

  // ConnectionMgr
  void setCM_CONNECTIONTYPE(connectionType_t connectionType);
  connectionType_t getCM_CONNECTIONTYPE() { return CM_CONNECTIONTYPE; }
  void setCM_WEBSOCKETTYPE(webSocketType_t webSocketType);
  webSocketType_t getCM_WEBSOCKETTYPE() { return CM_WEBSOCKETTYPE; }

  // WebServer
  uint16_t getWS_PORT() { return WS_PORT; }

  // Gondola
  Coordinate getGO_POSITION(void) { return GO_POSITION; }
  void setGO_POSITION(Coordinate position);
  Coordinate getGO_ANCHORPOS(void) { return GO_ANCHORPOS; }
  void setGO_ANCHORPOS(Coordinate position);
  float getGO_ROPEOFFSET(void) { return GO_ROPEOFFSET; }
  void setGO_ROPEOFFSET(float offset);

  // WebSocket
  uint16_t getWSO_PORT() { return WSO_PORT; }
  void setWSO_PORT(uint16_t port);

  // WebSocketServer

  // WebSocketClient
  std::string getWSO_HOST() { return WSO_HOST; }
  void setWSO_HOST(std::string host);

  // Debug
  logLevel_t getLOG_LEVEL() { return LOG_LEVEL; }
  void setLOG_LEVEL(logLevel_t level);

private:
  /**
   * persist a string to the EEPROM
   * If string doesn't use the maximum length a '\0' charachter will be appended
   * @param s         string to persist
   * @param start     start address in EEPRON
   * @param maxLength maximum length in EEPROM
   */
  void persistString(std::string &s, uint16_t start, uint8_t maxLength);

  /**
   * Read a string from a given address
   * @param s         string to read
   * @param start     start address in EERPON
   * @param maxLength maximum length of string
   */
  void readString(std::string &s, uint16_t start, uint8_t maxLength);

  /**
   * Write an IPAddres to the EEPROM
   * @param ip    IPAddress to save
   * @param start start address in EEPROM
   */
  void persistIPAddress(IPAddress &ip, uint16_t start);

  /**
   * Read an IPAddress from the EEPROM
   * @param ip    IPAddress to read
   * @param start start address in EEPROM
   */
  void readIPAddress(IPAddress &ip, uint16_t start);

  /**
   * Write a coordinate to the EEPROM
   * @param coord Coordinate to persist
   * @param start start address in EEPROM
   */
  void persistCoordinate(Coordinate &coord, uint16_t start);

  /**
   * Read a coordinate from the EEPROM
   * @param coord Coordinate to read
   * @param start start address in EEPROM
   */
  void readCoordinate(Coordinate &coord, uint16_t start);

  /**
   * Write a float to the EEPROM
   * @param f     float to persist
   * @param start start address in EEPROM
   */
  void persistFloat(float &f, uint16_t start);

  /**
   * Read a float from the EEPROM
   * @param f     Coordinate to read
   * @param start start address in EEPROM
   */
  void readFloat(float &f, uint16_t start);

  // Checksum
  /**
   * Calculates and erites the checksum to the EEPROM
   * @param start start address of checksum in EEPROM
   */
  void writeChecksum(uint16_t start);

  /**
   * Check the checksum from EEPROM with EEPROM Data
   * @param  start start address of checksum in EEPROM
   * @return       true if checksum is valid, false otherwise
   */
  bool checkChecksum(uint16_t start);

  // instance of Configuration
  static Config       *s_Instance;

  // WiFi Connection
  IPAddress WC_IPADDRESS;
  IPAddress WC_GATEWAY;
  IPAddress WC_NETMASK;
  std::string WC_SSID;
  std::string WC_PASSPHRASE;
  std::string WC_HOSTNAME;

  // Access Point
  IPAddress AP_IPADDRESS;
  IPAddress AP_GATEWAY;
  IPAddress AP_NETMASK;
  std::string AP_SSID;
  std::string AP_PASSPHRASE;
  std::string AP_URL;

  // ConnectionMgr
  connectionType_t CM_CONNECTIONTYPE;
  webSocketType_t CM_WEBSOCKETTYPE;

  // WebServer
  uint16_t WS_PORT;

  // Gondola
  Coordinate GO_POSITION;
  Coordinate GO_ANCHORPOS;
  float GO_ROPEOFFSET;

  // WebSocket
  uint16_t WSO_PORT;

  // WebSocketServer

  // WebSocketClient
  std::string WSO_HOST;

  // DEBUG
  logLevel_t LOG_LEVEL;
};


#endif /* _CONFIG_HPP_ */
