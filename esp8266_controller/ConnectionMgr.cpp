#include "ConnectionMgr.hpp"
#include "Config.hpp"
#include "WiFiConnection.hpp"
#include "APConnection.hpp"
#include "DualConnection.hpp"
#include "CommandInterpreter.hpp"
#include "Log.hpp"
#include "WebSocketServer.hpp"
#include "WebSocketClient.hpp"

ConnectionMgr *ConnectionMgr::s_Instance = NULL;

ConnectionMgr *ConnectionMgr::get()
{
  if (s_Instance == NULL)
  {
    s_Instance = new ConnectionMgr();
  }

  return s_Instance;
}

ConnectionMgr::ConnectionMgr()
  // Connection
 : m_ConnectionType(Config::get()->getCM_CONNECTIONTYPE())
 , m_ChangeConnectionType(CON_NONE)
 , m_ChangeConnectionRequest(false)
 , m_Connection(NULL)
 // WebSocket
 , m_WebSocketType(Config::get()->getCM_WEBSOCKETTYPE())
 , m_WebSocket(NULL)
 // WebServer
 , m_WebServer(Config::get()->getWS_PORT())
{
  s_Instance = this;          // set static variable here, since changeConnection creates a WebSocketServer which will use ConnectionMgr::get()
  CommandInterpreter::get()->addCommand(std::string("contype"), std::bind(&ConnectionMgr::contypeCommand, this, std::placeholders::_1));

  changeConnection(m_ConnectionType);
  changeWebSocket(m_WebSocketType);
}

ConnectionMgr::~ConnectionMgr()
{
  CommandInterpreter::get()->deleteCommand(std::string("contype"));
  if (m_Connection)
    delete(m_Connection);

  if (m_WebSocket)
    delete(m_WebSocket);

  s_Instance = NULL;
}

void ConnectionMgr::changeConnection(connectionType_t connectionType)
{
  m_ConnectionType = connectionType;
  Config *config = Config::get();

  // delete an old existing connection
  if (m_Connection != NULL)
  {
    delete(m_Connection);
    m_Connection = NULL;
  }

  delay(100);

  // initialize a new connection
  switch(m_ConnectionType)
  {
    case CON_ACCESS_POINT:
      m_Connection = new APConnection(config->getAP_SSID(), config->getAP_PASSPHRASE(), config->getAP_IPADDRESS(), config->getAP_GATEWAY(), config->getAP_NETMASK(), config->getAP_URL());
      break;

    case CON_WIFI_CONNECTION:
      m_Connection = new WiFiConnection(config->getWC_SSID(), config->getWC_PASSPHRASE(), config->getWC_HOSTNAME(), config->getWC_IPADDRESS(), config->getWC_GATEWAY(), config->getWC_NETMASK());
      break;

    case CON_DUAL_CONNECTION:
      m_Connection = new DualConnection(config->getAP_SSID(), config->getAP_PASSPHRASE(), config->getAP_IPADDRESS(), config->getAP_GATEWAY(), config->getAP_NETMASK(), config->getAP_URL(),
                                            config->getWC_SSID(), config->getWC_PASSPHRASE(), config->getWC_HOSTNAME(), config->getWC_IPADDRESS(), config->getWC_GATEWAY(), config->getWC_NETMASK());
      break;

    case CON_NONE:
      break;

    default:
      logWarning("Requested wrong Connection type!\n");
      break;
  }
}

void ConnectionMgr::requestChangeConnection(connectionType_t connectionType)
{
  m_ChangeConnectionRequest = true;
  m_ChangeConnectionType = connectionType;
}

connectionType_t ConnectionMgr::getConnectionType()
{
  return m_ConnectionType;
}

void ConnectionMgr::changeWebSocket(webSocketType_t webSocketType)
{
  m_WebSocketType = webSocketType;

  if (m_WebSocket != NULL)
  {
    delete(m_WebSocket);
    m_WebSocket = NULL;
  }

  switch (m_WebSocketType)
  {
    case WEBSOCKET_SERVER:
      m_WebSocket = new WebSocketServer(Config::get()->getWSO_PORT());
      break;

    case WEBSOCKET_CLIENT:
      m_WebSocket = new WebSocketClient(Config::get()->getWSO_HOST(), Config::get()->getWSO_PORT());
      break;

    case WEBSOCKET_NONE:
      break;

    default:
      logWarning("Requested wrong WebSocket type\n");
      break;
  }
}

webSocketType_t ConnectionMgr::getWebSocketType()
{
  return m_WebSocketType;
}

WebServer &ConnectionMgr::getWebServer()
{
  return m_WebServer;
}

void ConnectionMgr::loop()
{
  if (m_Connection)
    m_Connection->loop();

  m_WebServer.loop();

  if (m_WebSocket)
    m_WebSocket->loop();

  if (m_ChangeConnectionRequest)
  {
    m_ChangeConnectionRequest = false;
    changeConnection(m_ChangeConnectionType);
  }
}

bool ConnectionMgr::contypeCommand(std::string &s)
{
  CommandInterpreter *CI = CommandInterpreter::get();
  uint8_t args = CI->getNumArgument(s);

  if (args == 1)
  {
    std::string arg;
    CI->getArgument(s, arg, 0);
    if (arg.compare("WIFI") == 0)
    {
      Config::get()->setCM_CONNECTIONTYPE(CON_WIFI_CONNECTION);
      Config::get()->writeToEEPROM();
      changeConnection(CON_WIFI_CONNECTION);
      return true;
    }
    else if (arg.compare("AP") == 0)
    {
      Config::get()->setCM_CONNECTIONTYPE(CON_ACCESS_POINT);
      changeConnection(CON_ACCESS_POINT);
      Config::get()->writeToEEPROM();
      return true;
    }
    else if (arg.compare("DUAL") == 0)
    {
      Config::get()->setCM_CONNECTIONTYPE(CON_DUAL_CONNECTION);
      changeConnection(CON_DUAL_CONNECTION);
      Config::get()->writeToEEPROM();
      return true;
    }
    else if (arg.compare("NONE") == 0)
    {
      Config::get()->setCM_CONNECTIONTYPE(CON_NONE);
      changeConnection(CON_NONE);
      Config::get()->writeToEEPROM();
      return true;
    }
  }
  logWarning("Unsupported!\n");
  logWarning("Types are:\n");
  logWarning("\tAP\t- Access Point\n");
  logWarning("\tWIFI\t- Connect to a WiFi network\n");
  logWarning("\tDUAL\t- Connect to a WiFi network and open access point\n");
  logWarning("\tNONE\t- No WiFi connection\n");
  return false;
}

void ConnectionMgr::reset()
{
  delete(s_Instance);
  s_Instance = NULL;
  ConnectionMgr::get();
}
