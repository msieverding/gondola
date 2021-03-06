#include <functional>
#include "DualConnection.hpp"
#include "Log.hpp"
#include <functional>
#include "ConnectionMgr.hpp"

extern "C"
{
	#include "user_interface.h"
};

#define WIFI_CONNECTION_RETRIES   20

DualConnection::DualConnection(std::string ap_ssid, std::string ap_pw, IPAddress ap_ip, IPAddress ap_gw, IPAddress ap_nm, std::string ap_url,
                               std::string wc_ssid, std::string wc_pw, std::string wc_hostname, IPAddress wc_ip, IPAddress wc_gw, IPAddress wc_nm)
 : m_AP_SSID(ap_ssid)
 , m_AP_Passphrase(ap_pw)
 , m_AP_IPAddress(ap_ip)
 , m_AP_Gateway(ap_gw)
 , m_AP_Netmask(ap_nm)
 , m_AP_URL(ap_url)
 , m_AP_DnsServer()
 , m_WC_SSID(wc_ssid)
 , m_WC_Passphrase(wc_pw)
 , m_WC_IPAddress(wc_ip)
 , m_WC_Hostname(wc_hostname)
 , m_WC_Gateway(wc_gw)
 , m_WC_Netmask(wc_nm)
{
  logInfo("Starting Dual Mode\n");

  WiFi.mode(WIFI_AP_STA);
  WiFi.setOutputPower(20.5);            // set highest WiFi power
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);   // activate mixed WiFi

  if (setupWiFiConnection() == false)
  {
    ConnectionMgr::get()->requestChangeConnection(CON_ACCESS_POINT);
    WiFi.disconnect(false);
    WiFi.mode(WIFI_AP);
  }
  setupAccessPoint();
}

DualConnection::~DualConnection()
{
  m_AP_DnsServer.stop();
  WiFi.softAPdisconnect(false);
  WiFi.disconnect(false);
}

void DualConnection::loop()
{
  m_AP_DnsServer.processNextRequest();
}

bool DualConnection::setupWiFiConnection()
{
  logInfo("Setting up WiFi Connection\n");
  // Hostname
  WiFi.hostname(m_WC_Hostname.c_str());
  // DHCP is activated if ip = gw = nm = 0.0.0.0
  WiFi.config(m_WC_IPAddress, m_WC_Gateway, m_WC_Netmask);
  // Connect to Network
  WiFi.begin(m_WC_SSID.c_str(), m_WC_Passphrase.c_str());

  uint8_t tries = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (++tries == WIFI_CONNECTION_RETRIES)
    {
      logWarning("Not able to connect to WiFi Network %s. Go to access point only mode.\n", m_WC_SSID.c_str());
      return false;
    }
  }

  logInfo("WiFi connection established\n");
  m_WC_IPAddress = WiFi.localIP();
  // Print the IP address
  logInfo("Local IP address: %s\n", m_WC_IPAddress.toString().c_str());
  // MDNS
  return setupMDNS();
}

void DualConnection::setupAccessPoint()
{
  logInfo("Setting Soft-AP configuration... ");
  logInfo(WiFi.softAPConfig(m_AP_IPAddress, m_AP_Gateway, m_AP_Netmask) ? "Ready\n" : "Failed!\n");

  // Set maximum clients to 8
  struct softap_config config;
  wifi_softap_get_config(&config);    // Get config first.
  config.max_connection = 8;          // how many stations can connect to ESP8266 softAP at most.
  wifi_softap_set_config(&config);    // Set ESP8266 softap config

  logInfo("Setting Soft-AP... ");
  if (m_AP_Passphrase.length() >= 8 && m_AP_Passphrase.length() <= 63)
    logInfo(WiFi.softAP(m_AP_SSID.c_str(), m_AP_Passphrase.c_str()) ? "Ready\n" : "Failed!\n");
  else
    logInfo(WiFi.softAP(m_AP_SSID.c_str()) ? "Ready\n" : "Failed!\n");

  logInfo("Soft-AP IP address: %s\n", WiFi.softAPIP().toString().c_str());

  logInfo("Soft-AP AP SSID: %s\n", m_AP_SSID.c_str());

  if (m_AP_Passphrase.length() > 8 && m_AP_Passphrase.length() < 32)
  {
    logInfo("Connect with passphrase: %s\n", m_AP_Passphrase.c_str());
  }
  else
  {
    logInfo("No passphrase used\n");
  }
  // DNSServer
  setupDNS();
}

void DualConnection::setupDNS()
{
  // Setup DNS server for easy access without IP address
  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  m_AP_DnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  m_AP_DnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
  m_AP_DnsServer.start(53, m_AP_URL.c_str(), m_AP_IPAddress);

  logInfo("You can access gondola's main page with: http://\%s/\n", m_AP_URL.c_str());
}

bool DualConnection::setupMDNS()
{
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(m_WC_Hostname.c_str()))
  {
    logWarning("Error setting up mDNS responder!");
    return false;
  }
  logInfo("Use this URL to connect (mDNS): http://%s.local/\n", m_WC_Hostname.c_str());

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", Config::get()->getWS_PORT());
  return true;
}
