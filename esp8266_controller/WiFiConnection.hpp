#ifndef _WIFI_CONNECTION_HPP_
#define _WIFI_CONNECTION_HPP_

#include <ESP8266WiFi.h>
#include "Gondola.hpp"
#include "WebServer.hpp"
#include "IConnection.hpp"
#include <ESP8266mDNS.h>

/**
 * Establish a WiFi connection to a WiFi network.
 */
class WiFiConnection : public IConnection
{
public:
  /**
   * Constructor
   * @param  ssid       SSID of network
   * @param  passphrase Passphrase for network with ssid
   * @param  hostname   Hostname to use during connection
   * @param  ip         IP address to use during static IP operation / 0.0.0.0 for DHCP
   * @param  gw         Gateway to use during static IP operation / 0.0.0.0 for DHCP
   * @param  nm         Netmask to use during static IP operation / 0.0.0.0 for DHCP
   */
  WiFiConnection(std::string ssid, std::string passphrase, std::string hostname, IPAddress ip, IPAddress gw, IPAddress nm);

  /**
   * Virtual destructor
   */
  virtual ~WiFiConnection();

private:
  /**
   * Setup the MDNS functionality
   * Needed host software:
   * - Linux: Avahi (http://avahi.org/)
   * - Windows: Bonjour (http://www.apple.com/support/bonjour/)
   * - Max OSX and iOs is built in through Bonjour already
   */
  void setupMDNS();

  /**
   * Event handler when the WiFi connection gets an IP
   * @param event Event to handle
   */
  void onEventGotIP(const WiFiEventStationModeGotIP &event);

  /**
   * Event handler when the WiFi connection is established
   * @param event Event to handle
   */
  void onEventConnected(const WiFiEventStationModeConnected &event);

  /**
   * Event handler when the WiFi connection gets lost
   * @param event Event to handle
   */
  void onEventDisconnected(const WiFiEventStationModeDisconnected &event);

  // Membervariables
  std::string                m_SSID;                        //!< SSID to connect to
  std::string                m_Passphrase;                  //!< Passphrase to use for the network
  IPAddress                  m_IPAddress;                   //!< IPaddres to use / 0.0.0.0 for DHCP
  std::string                m_Hostname;                    //!< Hostname for this chip
  IPAddress                  m_Gateway;                     //!< Gateway to use / 0.0.0.0 for DHCP
  IPAddress                  m_Netmask;                     //!< Netmask to use / 0.0.0.0 for DHCP
  WiFiEventHandler           m_StationGotIPHandler;         //!< Handler is called, when this station got an IP
  WiFiEventHandler           m_StationConnectedHandler;     //!< Handler is called, when this station gets connection
  WiFiEventHandler           m_StationDisconnectedHandler;  //!< Handler is called, when this station looses connection
};

#endif /* _WIFI_CONNECTION_HPP_ */
