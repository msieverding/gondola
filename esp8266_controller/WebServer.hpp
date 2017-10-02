#ifndef _WEB_SERVER_HPP_
#define _WEB_SERVER_HPP_

#include <ESP8266WebServer.h>
#include "Gondola.hpp"

/**
 * Webserver provides HTTP server with a website to access all functions
 * of gondola through a webserver and to configure the system.
 */
class WebServer
{
public:
  /**
   * Private constructor
   * @param  port               Port to use for the server
   * @param  configureServer    constructor configures server if true
   */
  WebServer(uint16_t port, bool configureServer = true);

  /**
   * Virtual desctructor
   */
  virtual ~WebServer();

  /**
   * Call this loop periodically to handle WebServer
   */
  virtual void loop();

  /**
   * regiter a gondola to the webserer
   * @param gondola pointer to gondola
   */
  void registerGondola(Gondola *gondola);

protected:
  // HTTP Handler
  virtual void handleRoot();
  virtual void handleSetupWiFi();
  virtual void handleSetupSystem();
  virtual void handleShowAPClients();
  virtual void handleNotFound();
  virtual void handleConsole();

  // Helper functions to build web pages
  virtual void prepareHeader(std::string &s);
  virtual void prepareFooter(std::string &s);
  void prepareSetupWiFiPage(std::string &s);
  void prepareSetupSystemPage(std::string &s);
  void prepareGondolaMovePage(std::string &s);
  void prepareShowAPClientsPage(std::string &s);
  void prepareConsolePage(std::string &s);

  // Membervariables
  ESP8266WebServer          m_Server;   //!< Web server to handle
  Gondola                  *m_Gondola;  //!< registered gondola to use for the web pages
};
#endif /* _WEB_SERVER_HPP_ */
