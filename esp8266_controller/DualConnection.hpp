#ifndef _DUAL_CONNECTION_
#define _DUAL_CONNECTION_

#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <IPAddress.h>
#include "ESP8266WiFiType.h"

#include "IConnection.hpp"
#include "WebServer.hpp"

/**
 * Class for a WiFi Connection and an Access Point at the same time
 */
class DualConnection : public IConnection
{
public:
  /**
   * Constructor of the dual connection
   * @param  ap_ssid     SSID for the AP
   * @param  ap_pw       Passphrase for the AP
   * @param  ap_ip       IP for the AP
   * @param  ap_gw       Gateway for the AP
   * @param  ap_nm       Netmask for the AP
   * @param  ap_url      URL for the AP
   * @param  wc_ssid     SSID of the network to connect
   * @param  wc_pw       Passphrase of the network to connect
   * @param  wc_hostname Hostname of the ESP8266 in this network
   * @param  wc_ip       Static IP to use / 0.0.0.0 for DHCP
   * @param  wc_gw       Static gateway to use / 0.0.0.0 for DHCP
   * @param  wc_nm       Static netmask to use / 0.0.0.0 for DHCP
   */
  DualConnection(std::string ap_ssid, std::string ap_pw, IPAddress ap_ip, IPAddress ap_gw, IPAddress ap_nm, std::string ap_url,
                 std::string wc_ssid, std::string wc_pw, std::string wc_hostname, IPAddress wc_ip, IPAddress wc_gw, IPAddress wc_nm);

  /**
   * Virtual destructor
   */
  virtual ~DualConnection();

  /**
   * Loop to call periodically
   */
  virtual void loop();

private:
  /**
   * Setup the WiFi Connection
   */
  void setupWiFiConnection();

  /**
   * Setup mDNS for the WiFi connection
   */
  void setupMDNS();

  /**
   * Event handler when the WiFi connection gets an IP
   * @param event event to handle
   */
  void onEventGotIP(const WiFiEventStationModeGotIP &event);

  /**
   * Setup the access point
   */
  void setupAccessPoint();

  /**
   * Setup a DNS Server for the access point
   */
  void setupDNS();

  // Membervariables
  // Access point
  std::string              m_AP_SSID;                 //!< Name for access point
  std::string              m_AP_Passphrase;           //!< Passphrase for access point
  IPAddress                m_AP_IPAddress;            //!< IP Address of this chip
  IPAddress                m_AP_Gateway;              //!< Gateway to use
  IPAddress                m_AP_Netmask;              //!< Netmask of network
  std::string              m_AP_URL;                  //!< URL of this chip
  DNSServer                m_AP_DnsServer;            //!< DNS Server to provide
  // WiFi connection
  std::string              m_WC_SSID;                 //!< SSID of network to connect to
  std::string              m_WC_Passphrase;           //!< Passphrase for Network SSID
  IPAddress                m_WC_IPAddress;            //!< IPAddress of this chip
  std::string              m_WC_Hostname;             //!< Hostname of this chip in this connection
  IPAddress                m_WC_Gateway;              //!< Gateway to use
  IPAddress                m_WC_Netmask;              //!< Netmask of this network
  WiFiEventHandler         m_WC_StationGotIPHandler;  //!< Handler is called, when this station got an IP


};
#endif /* _DUAL_CONNECTION_ */
