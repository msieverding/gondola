#include "APConnection.hpp"
#include "config.hpp"
#include "Log.hpp"

APConnection::APConnection(std::string ssid, std::string passphrase, IPAddress ip, IPAddress gateway, IPAddress netmask, std::string url)
 : m_SSID(ssid)
 , m_Passphrase(passphrase)
 , m_IPAddress(ip)
 , m_Gateway(gateway)
 , m_Netmask(netmask)
 , m_DnsServer()
 , m_URL(url)
{
  WiFi.mode(WIFI_AP);
  WiFi.setOutputPower(20.5);            // set highest WiFi power
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);   // activate mixed WiFi

  logInfo("Setting soft-AP configuration... ");
  logInfo(WiFi.softAPConfig(m_IPAddress, m_Gateway, m_Netmask) ? "Ready\n" : "Failed!\n");

  logInfo("Setting soft-AP... ");
  if (m_Passphrase.length() >= 8 && m_Passphrase.length() <= 63)
    logInfo(WiFi.softAP(m_SSID.c_str(), m_Passphrase.c_str()) ? "Ready\n" : "Failed!\n");
  else
    logInfo(WiFi.softAP(m_SSID.c_str()) ? "Ready\n" : "Failed!\n");

  logInfo("Soft-AP IP address: %s\n", WiFi.softAPIP().toString().c_str());

  logInfo("WiFi AP SSID: %s\n", m_SSID.c_str());

  if (m_Passphrase.length() > 8 && m_Passphrase.length() < 32)
  {
    logInfo("Connect with passphrase: %s\n", m_Passphrase.c_str());
  }
  else
  {
    logInfo("No passphrase used\n");
  }

  setupDNS();
}

APConnection::~APConnection()
{
  m_DnsServer.stop();
  WiFi.softAPdisconnect(false);
  WiFi.disconnect(false);
}

void APConnection::loop()
{
  m_DnsServer.processNextRequest();
}

void APConnection::setupDNS()
{
  // Setup DNS server for easy access without IP address
  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  m_DnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  m_DnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
  m_DnsServer.start(53, m_URL.c_str(), m_IPAddress);

  logInfo("You can access gondola's main page with: http://\%s/\n", m_URL.c_str());
}
