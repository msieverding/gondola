#ifndef _AP_CONNECTION_HPP_
#define _AP_CONNECTION_HPP_

#include "IConnection.hpp"
#include "WebServer.hpp"
#include <DNSServer.h>

/**
 * Class to open an access point
 */
class APConnection : public IConnection
{
public:
  /**
   * Constructor
   * @param  ssid       SSID of the access point
   * @param  passphrase Passphrase of the access point
   * @param  ip         IP Address of the chip
   * @param  gateway    Gateway to use
   * @param  netmask    Netmask of the network
   * @param  url        URL to provide
   * @return            Pointer to instance of access point
   */
  APConnection(std::string ssid, std::string passphrase, IPAddress ip, IPAddress gateway, IPAddress netmask, std::string url);

  /**
   * Virtual destructor
   */
  virtual ~APConnection();

  /**
   * Call this frequently to handle the DNSServer
   */
  virtual void loop();

private:
  /**
   * Setup the DNS Server
   */
  void setupDNS();

  // Membervariables
  std::string              m_SSID;              //!< SSID of the provided network
  std::string              m_Passphrase;        //!< Passphrase of te provided netowkr
  IPAddress                m_IPAddress;         //!< IP Address of this Chip
  IPAddress                m_Gateway;           //!< Gateway to use for this connrction
  IPAddress                m_Netmask;           //!< Netmask of the provided network
  DNSServer                m_DnsServer;         //!< DNS Server for the provided network
  std::string              m_URL;               //!< URL to access this chip
};

#endif /* _AP_CONNECTION_HPP_ */
