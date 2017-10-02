#include "Config.hpp"
#include <EEPROM.h>
#include "ConnectionMgr.hpp"
#include "CommandInterpreter.hpp"
#include "Log.hpp"

#define EEPROM_LENGTH                   512

#define EEPROM_WC_SSID_START            0
#define EEPROM_WC_SSID_LENGTH           40
#define EEPROM_WC_PASSPHRASAE_START     40
#define EEPROM_WC_PASSPHRASE_LENGTH     40
#define EEPROM_WC_IPADDRESS_START       80
#define EEPROM_WC_IPADDRESS_LENGTH      4
#define EEPROM_WC_GATEWAY_START         84
#define EEPROM_WC_GATEWAY_LENGTH        4
#define EEPROM_WC_NETMASK_START         88
#define EEPROM_WC_NETMASAK_LENGTH       4
#define EEPROM_WC_HOSTNAME_START        92
#define EEPROM_WC_HOSTNAME_LENGTH       40
// Placeholder for more features until address 149

#define EEPROM_AP_SSID_START            150
#define EEPROM_AP_SSID_LENGTH           40
#define EEPROM_AP_PASSPHRASAE_START     190
#define EEPROM_AP_PASSPHRASE_LENGTH     40
#define EEPROM_AP_IPADDRESS_START       230
#define EEPROM_AP_IPADDRESS_LENGTH      4
#define EEPROM_AP_GATEWAY_START         234
#define EEPROM_AP_GATEWAY_LENGTH        4
#define EEPROM_AP_NETMASK_START         238
#define EEPROM_AP_NETMASAK_LENGTH       4
#define EEPROM_AP_URL_START             242
#define EEPROM_AP_URL_LENGTH            40
// Placeholder for more features until address 299

// ConnectionMgr
#define EEPROM_CM_CONNECIONTYPE_START   300
#define EEPROM_CM_WEBSOCKETTYPE_START   301

// Gondola
#define EEPROM_GO_POSITION_START        302
#define EEPROM_GO_ANCHORPOS_START       314
#define EEPROM_GO_ROPEOFFSET_START      326

// WebSocketClient
#define EEPROM_WSO_HOST_START           330
#define EEPROM_WSO_HOST_LENGTH          20

// Debug
#define EEPROM_LOG_LEVEL_START          350

// Checksum
#define EEPROM_CHECKSUM_DATA_BEGIN      0
#define EEPROM_CHECKSUM_DATA_END        350
#define EEPROM_CHECKSUM_START           351

Config *Config::s_Instance = NULL;

Config *Config::get()
{
  if (!s_Instance)
    s_Instance = new Config();

  return s_Instance;
}

Config::Config()
 // WiFi Connection
 : WC_IPADDRESS(0, 0, 0, 0)
 , WC_GATEWAY(0, 0, 0, 0)
 , WC_NETMASK(0, 0, 0, 0)
 , WC_SSID("GondolaWiFi")
 , WC_PASSPHRASE("TU_GRAZ_ITI")
 , WC_HOSTNAME("gondola")
 // Access Point
 , AP_IPADDRESS(192, 168, 4, 1)
 , AP_GATEWAY(192, 168, 4, 1)
 , AP_NETMASK(255, 255, 255, 0)
 , AP_SSID("GondolaWiFi")
 , AP_PASSPHRASE("TU_GRAZ_ITI")
 , AP_URL("www.gondola.com")
 // ConnectionMgr Setup
 , CM_CONNECTIONTYPE(CON_ACCESS_POINT)
 , CM_WEBSOCKETTYPE(WEBSOCKET_NONE)
 // WebServer
 , WS_PORT(80)
 // Gondola
 , GO_POSITION(0.0, 0.0, 0.0)
 , GO_ANCHORPOS(0.0, 0.0, 0.0)
 , GO_ROPEOFFSET(0.0f)
 // WebSocket
 , WSO_PORT(443)
 // WebSocketServer
 // - none yet
 // WebSocketClient
 , WSO_HOST("www.gondola.com")
 // Logging
 , LOG_LEVEL(LOG_VERBOSE)
{
  EEPROM.begin(EEPROM_LENGTH);
}

Config::~Config()
{
  EEPROM.end();
  s_Instance = NULL;
}

void Config::resetConfig()
{
  if (s_Instance)
  {
    delete(s_Instance);
    s_Instance = new Config();
    s_Instance->writeToEEPROM();
    s_Instance->printConfig();
  }
}

bool Config::writeToEEPROM()
{
  // WiFi Connection Setup
  writeWCToEEPROM(false);

  // Access Point Setup
  writeAPToEEPROM(false);

  EEPROM.write(EEPROM_CM_CONNECIONTYPE_START, static_cast<uint8_t>(CM_CONNECTIONTYPE));
  EEPROM.write(EEPROM_CM_WEBSOCKETTYPE_START, static_cast<uint8_t>(CM_WEBSOCKETTYPE));

  writeGOToEEPROM(false);

  persistString(WSO_HOST, EEPROM_WSO_HOST_START, EEPROM_WSO_HOST_LENGTH);

  EEPROM.write(EEPROM_LOG_LEVEL_START, static_cast<uint8_t>(LOG_LEVEL));

  writeChecksum(EEPROM_CHECKSUM_START);

  return EEPROM.commit();
}

bool Config::writeWCToEEPROM(bool persist)
{
  // WiFi Connection Setup
  persistString(    WC_SSID,        EEPROM_WC_SSID_START,           EEPROM_WC_SSID_LENGTH);
  persistString(    WC_PASSPHRASE,  EEPROM_WC_PASSPHRASAE_START,    EEPROM_WC_PASSPHRASE_LENGTH);
  persistIPAddress( WC_IPADDRESS,   EEPROM_WC_IPADDRESS_START);
  persistIPAddress( WC_GATEWAY,     EEPROM_WC_GATEWAY_START);
  persistIPAddress( WC_NETMASK,     EEPROM_WC_NETMASK_START);
  persistString(    WC_HOSTNAME,    EEPROM_WC_HOSTNAME_START,       EEPROM_WC_HOSTNAME_LENGTH);

  if (persist)
  {
    writeChecksum(EEPROM_CHECKSUM_START);
    return EEPROM.commit();
  }
  else
  {
    return true;
  }
}

bool Config::writeAPToEEPROM(bool persist)
{
  // Access Point Setup
  persistString(    AP_SSID,        EEPROM_AP_SSID_START,           EEPROM_AP_SSID_LENGTH);
  persistString(    AP_PASSPHRASE,  EEPROM_AP_PASSPHRASAE_START,    EEPROM_AP_PASSPHRASE_LENGTH);
  persistIPAddress( AP_IPADDRESS,   EEPROM_AP_IPADDRESS_START);
  persistIPAddress( AP_GATEWAY,     EEPROM_AP_GATEWAY_START);
  persistIPAddress( AP_NETMASK,     EEPROM_AP_NETMASK_START);
  persistString(    AP_URL,         EEPROM_AP_URL_START,            EEPROM_AP_URL_LENGTH);

  if (persist)
  {
    writeChecksum(EEPROM_CHECKSUM_START);
    return EEPROM.commit();
  }
  else
  {
    return true;
  }
}

bool Config::writeGOToEEPROM(bool persist)
{
  // Gondola setup
  persistCoordinate(GO_POSITION,      EEPROM_GO_POSITION_START);
  persistCoordinate(GO_ANCHORPOS,     EEPROM_GO_ANCHORPOS_START);
  persistFloat(     GO_ROPEOFFSET,    EEPROM_GO_ROPEOFFSET_START);

  if (persist)
  {
    writeChecksum(EEPROM_CHECKSUM_START);
    return EEPROM.commit();
  }
  else
  {
    return true;
  }
}

void Config::readFromEEPROM()
{
  // Check validity of data
  if (checkChecksum(EEPROM_CHECKSUM_START) == false)
  {
    logWarning("Checksum not valid! Use default values!\n");
    printConfig();
    return;
  }
  else
  {
    logInfo("Checksum valid! Load Data from EEPROM!\n");
  }
  // WiFI Connection Setup
  readString(    WC_SSID,        EEPROM_WC_SSID_START,           EEPROM_WC_SSID_LENGTH);
  readString(    WC_PASSPHRASE,  EEPROM_WC_PASSPHRASAE_START,    EEPROM_WC_PASSPHRASE_LENGTH);
  readIPAddress( WC_IPADDRESS,   EEPROM_WC_IPADDRESS_START);
  readIPAddress( WC_GATEWAY,     EEPROM_WC_GATEWAY_START);
  readIPAddress( WC_IPADDRESS,   EEPROM_WC_IPADDRESS_START);
  readString(    WC_HOSTNAME,    EEPROM_WC_HOSTNAME_START,       EEPROM_WC_HOSTNAME_LENGTH);

  // Access Point Setup
  readString(    AP_SSID,        EEPROM_AP_SSID_START,           EEPROM_AP_SSID_LENGTH);
  readString(    AP_PASSPHRASE,  EEPROM_AP_PASSPHRASAE_START,    EEPROM_AP_PASSPHRASE_LENGTH);
  readIPAddress( AP_IPADDRESS,   EEPROM_AP_IPADDRESS_START);
  readIPAddress( AP_GATEWAY,     EEPROM_AP_GATEWAY_START);
  readIPAddress( AP_NETMASK,     EEPROM_AP_NETMASK_START);
  readString(    AP_URL,         EEPROM_AP_URL_START,            EEPROM_AP_URL_LENGTH);

  CM_CONNECTIONTYPE = static_cast<connectionType_t>(EEPROM.read(EEPROM_CM_CONNECIONTYPE_START));
  CM_WEBSOCKETTYPE = static_cast<webSocketType_t>(EEPROM.read(EEPROM_CM_WEBSOCKETTYPE_START));

  readCoordinate(GO_POSITION,      EEPROM_GO_POSITION_START);
  readCoordinate(GO_ANCHORPOS,     EEPROM_GO_ANCHORPOS_START);
  readFloat(     GO_ROPEOFFSET,    EEPROM_GO_ROPEOFFSET_START);

  readString(    WSO_HOST,         EEPROM_WSO_HOST_START,        EEPROM_WSO_HOST_LENGTH);

  LOG_LEVEL = static_cast<logLevel_t>(EEPROM.read(EEPROM_LOG_LEVEL_START));

  printConfig();
}

void Config::persistString(std::string &s, uint16_t start, uint8_t maxLength)
{
  int length = s.length();
  uint8_t byte;

  // Figure out max length
  if (length > maxLength)
    length = maxLength;

  // write string bytewise
  for (byte = 0; byte < length; byte++)
    EEPROM.write(start + byte, static_cast<uint8_t>(s[byte]));

  // when string has not max length add end sequence
  if (length < maxLength)
    EEPROM.write(start + byte, static_cast<uint8_t>('\0'));
}

void Config::readString(std::string &s, uint16_t start, uint8_t maxLength)
{
  uint8_t byte;
  char * str = new char[maxLength + 1];

  s.clear();
  for (byte = 0; byte < maxLength; byte++)
  {
    char c = EEPROM.read(start + byte);
    if (c != '\0')
      str[byte] = c;
    else
      break;
  }
  str[byte] = '\0';
  s.append(str);
}

void Config::persistIPAddress(IPAddress &ip, uint16_t start)
{
  for (uint8_t i = 0; i < 4; i++)
    EEPROM.write(start + i, ip[i]);
}

void Config::readIPAddress(IPAddress &ip, uint16_t start)
{
  for (uint8_t i = 0; i < 4; i++)
    ip[i] = EEPROM.read(start + i);
}

void Config::persistCoordinate(Coordinate &coord, uint16_t start)
{
  persistFloat(coord.x, start);
  persistFloat(coord.y, start + 4);
  persistFloat(coord.z, start + 8);
}

void Config::readCoordinate(Coordinate &coord, uint16_t start)
{
  readFloat(coord.x, start);
  readFloat(coord.y, start + 4);
  readFloat(coord.z, start + 8);
}

void Config::persistFloat(float &f, uint16_t start)
{
  b4Converter_t converter;
  converter.f = f;
  EEPROM.write(start, converter.b[0]);
  EEPROM.write(start + 1, converter.b[1]);
  EEPROM.write(start + 2, converter.b[2]);
  EEPROM.write(start + 3, converter.b[3]);
}

void Config::readFloat(float &f, uint16_t start)
{
  b4Converter_t converter;
  converter.b[0] = EEPROM.read(start);
  converter.b[1] = EEPROM.read(start + 1);
  converter.b[2] = EEPROM.read(start + 2);
  converter.b[3] = EEPROM.read(start + 3);
  f = converter.f;
}

void Config::writeChecksum(uint16_t start)
{
  // Fletcher checksum
  uint8_t chk1 = 0, chk2 = 0;
  uint16_t i;
  for (i = EEPROM_CHECKSUM_DATA_BEGIN; i <= EEPROM_CHECKSUM_DATA_END; i++)
  {
    chk1 = chk1 + EEPROM.read(i);
    chk2 = chk2 + chk1;
  }
  EEPROM.write(EEPROM_CHECKSUM_START, chk1);
  EEPROM.write(EEPROM_CHECKSUM_START + 1, chk2);
}

bool Config::checkChecksum(uint16_t start)
{
  // Fletcher checksum
  uint8_t chk1 = 0, chk2 = 0;
  uint8_t chk1EEPROM = EEPROM.read(EEPROM_CHECKSUM_START);
  uint8_t chk2EEPROM = EEPROM.read(EEPROM_CHECKSUM_START + 1);
  uint16_t i;

  for (i = EEPROM_CHECKSUM_DATA_BEGIN; i <= EEPROM_CHECKSUM_DATA_END; i++)
  {
    chk1 = chk1 + EEPROM.read(i);
    chk2 = chk2 + chk1;
  }

  if (chk1 == chk1EEPROM && chk2 == chk2EEPROM)
    return true;
  else
    return false;
}

void Config::printConfig(void)
{
  logDebug("Loaded configuration:\n");
  // WiFi Connection
  logDebug("WC_IPADDRESS:       %s\n", WC_IPADDRESS.toString().c_str());
  logDebug("WC_GATEWAY:         %s\n", WC_GATEWAY.toString().c_str());
  logDebug("WC_NETMASK:         %s\n", WC_NETMASK.toString().c_str());
  logDebug("WC_SSID:            %s\n", WC_SSID.c_str());
  logDebug("WC_PASSPHRASE:      %s\n", WC_PASSPHRASE.c_str());
  logDebug("WC_HOSTNAME:        %s\n", WC_HOSTNAME.c_str());
  // Access point
  logDebug("AP_IPADDRESS:       %s\n", AP_IPADDRESS.toString().c_str());
  logDebug("AP_GATEWAY:         %s\n", AP_GATEWAY.toString().c_str());
  logDebug("AP_NETMASK:         %s\n", AP_NETMASK.toString().c_str());
  logDebug("AP_SSID:            %s\n", AP_SSID.c_str());
  logDebug("AP_PASSPHRASE:      %s\n", AP_PASSPHRASE.c_str());
  logDebug("AP_URL:             %s\n", AP_URL.c_str());
  logDebug("WC_HOSTNAME:        %s\n", WC_HOSTNAME.c_str());
  // ConnectionMgr Setup
  logDebug("CM_CONNECTIONTYPE:  %u\n", static_cast<uint8_t>(CM_CONNECTIONTYPE));
  logDebug("CM_WEBSOCKETTYPE:   %u\n", static_cast<uint8_t>(CM_WEBSOCKETTYPE));
  // WebServer
  logDebug("WS_PORT:            %u\n", WS_PORT);
  // Gondola
  logDebug("GO_POSITION:        %s\n", GO_POSITION.toString().c_str());
  logDebug("GO_ANCHORPOS:       %s\n", GO_ANCHORPOS.toString().c_str());
  logDebug("GO_ROPEOFFSET       %s\n", FTOS(GO_ROPEOFFSET));
  // WebSocket
  logDebug("WSO_PORT:           %u\n", WSO_PORT);
  logDebug("WSO_HOST:           %s\n", WSO_HOST.c_str());
  logDebug("LOG_LEVEL:          %u\n", static_cast<uint8_t>(LOG_LEVEL));
}

// WiFi Connection
void Config::setWC_IPADDRESS(IPAddress ip)
{
  WC_IPADDRESS = ip;
}

void Config::setWC_GATEWAY(IPAddress gw)
{
  WC_GATEWAY = gw;
}

void Config::setWC_NETMASK(IPAddress nm)
{
  WC_NETMASK = nm;
}

void Config::setWC_IPADDRESS(String ip)
{
  IPAddress tmp;
  tmp.fromString(ip);
  setWC_IPADDRESS(tmp);
}

void Config::setWC_GATEWAY(String gw)
{
  IPAddress tmp;
  tmp.fromString(gw);
  setWC_GATEWAY(tmp);
}

void Config::setWC_NETMASK(String nm)
{
  IPAddress tmp;
  tmp.fromString(nm);
  setWC_NETMASK(tmp);
}

void Config::setWC_SSID(std::string ssid)
{
  WC_SSID = ssid;
}

void Config::setWC_PASSPHRASE(std::string passphrase)
{
  WC_PASSPHRASE = passphrase;
}

void Config::setWC_HOSTNAME(std::string name)
{
  WC_HOSTNAME = name;
}

// Access Point

void Config::setAP_IPADDRESS(IPAddress ip)
{
  AP_IPADDRESS = ip;
}

void Config::setAP_GATEWAY(IPAddress gw)
{
  AP_GATEWAY = gw;
}

void Config::setAP_NETMASK(IPAddress nm)
{
  AP_NETMASK = nm;
}

void Config::setAP_IPADDRESS(String ip)
{
  IPAddress tmp;
  tmp.fromString(ip);
  setAP_IPADDRESS(tmp);
}

void Config::setAP_GATEWAY(String gw)
{
  IPAddress tmp;
  tmp.fromString(gw);
  setAP_GATEWAY(tmp);
}

void Config::setAP_NETMASK(String nm)
{
  IPAddress tmp;
  tmp.fromString(nm);
  setAP_NETMASK(tmp);
}

void Config::setAP_SSID(std::string ssid)
{
  AP_SSID = ssid;
}

void Config::setAP_PASSPHRASE(std::string passphrase)
{
  AP_PASSPHRASE = passphrase;
}

void Config::setAP_URL(std::string url)
{
  AP_URL = url;
}

// ConnectionMgr
void Config::setCM_CONNECTIONTYPE(connectionType_t connectionType)
{
  CM_CONNECTIONTYPE = connectionType;
}

void Config::setCM_WEBSOCKETTYPE(webSocketType_t webSocketType)
{
  CM_WEBSOCKETTYPE = webSocketType;
}

// Gondola
void Config::setGO_POSITION(Coordinate position)
{
  GO_POSITION = position;
}

void Config::setGO_ANCHORPOS(Coordinate position)
{
  GO_ANCHORPOS = position;
}

void Config::setGO_ROPEOFFSET(float offset)
{
  GO_ROPEOFFSET = offset;
}

// WebSocket
void Config::setWSO_PORT(uint16_t port)
{
  WSO_PORT = port;
}
// WebSocketServer

// WebSocketClient
void Config::setWSO_HOST(std::string host)
{
  WSO_HOST = host;
}

// Debug
void Config::setLOG_LEVEL(logLevel_t level)
{
  LOG_LEVEL = level;
}
