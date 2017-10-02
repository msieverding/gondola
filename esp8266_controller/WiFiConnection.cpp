#include "WiFiConnection.hpp"
#include "ConnectionMgr.hpp"
#include "Log.hpp"
#include <functional>

WiFiConnection::WiFiConnection(std::string ssid, std::string passphrase, std::string hostname, IPAddress ip, IPAddress gw, IPAddress nm)
 : m_SSID(ssid)
 , m_Passphrase(passphrase)
 , m_IPAddress(ip)
 , m_Hostname(hostname)
 , m_Gateway(gw)
 , m_Netmask(nm)
{
  WiFi.mode(WIFI_STA);
  WiFi.setOutputPower(20.5);            // set highest WiFi power
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);   // activate mixed WiFi

  // Set hostname for Client
  // https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/station-class.rst#disconnect
  WiFi.hostname(m_Hostname.c_str());

  // DHCP is activated if ip = gw = nm = 0.0.0.0
  WiFi.config(m_IPAddress, m_Gateway, m_Netmask);
  // Connect to Network
  WiFi.begin(m_SSID.c_str(), m_Passphrase.c_str());
  // Set stationary mode

  logInfo("Wait for connect to WiFi: %s\n", m_SSID.c_str());

  // Event for connection changes
  m_StationGotIPHandler = WiFi.onStationModeGotIP(std::bind(&WiFiConnection::onEventGotIP, this, std::placeholders::_1));
  m_StationConnectedHandler = WiFi.onStationModeConnected(std::bind(&WiFiConnection::onEventConnected, this, std::placeholders::_1));
  m_StationDisconnectedHandler = WiFi.onStationModeDisconnected(std::bind(&WiFiConnection::onEventDisconnected, this, std::placeholders::_1));
}

WiFiConnection::~WiFiConnection()
{
  WiFi.disconnect(false);
}

void WiFiConnection::setupMDNS()
{
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(m_Hostname.c_str()))
  {
    logWarning("Error setting up mDNS responder!\n");
    return;
  }
  logInfo("Use this URL to connect (mDNS): http://%s.local/\n", m_Hostname.c_str());

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", Config::get()->getWS_PORT());
}

void WiFiConnection::onEventGotIP(const WiFiEventStationModeGotIP &event)
{
  logInfo("WiFi connection got IP\n");
  m_IPAddress = WiFi.localIP();
  // Print the IP address
  logInfo("Local IP address: %s\n", m_IPAddress.toString().c_str());

  // MDNS
  setupMDNS();
}

void WiFiConnection::onEventConnected(const WiFiEventStationModeConnected &event)
{
  logDebug("WiFi connection established\n");
}

void WiFiConnection::onEventDisconnected(const WiFiEventStationModeDisconnected &event)
{
  logDebug("WiFi connection lost\n");
}
