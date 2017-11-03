// Configuration
#include "Config.hpp"
// Own classes and libraries
#include "Gondola.hpp"
#include "WiFiConnection.hpp"
#include "SerialConnection.hpp"
#include "APConnection.hpp"
#include "WebServer.hpp"
#include "CommandInterpreter.hpp"
#include "ConnectionMgr.hpp"
#include "Log.hpp"

#include "ApplicationRSSIMeasServer.hpp"
#include "ApplicationRSSIMeasClient.hpp"

IConnection *serial;

ApplicationRSSIMeasServer *RSSIServer;
ApplicationRSSIMeasClient *RSSIClient;

void setup()
{
  // Wait one second for hardware and serial console
  delay(1000);
  serial = SerialConnection::create(115200);

  Config::get()->readFromEEPROM();
  setLogLevel(Config::get()->getLOG_LEVEL());

  ConnectionMgr::get();

  wdt_enable(1000);
  // ESP.wdtEnable(1000);

  // TODO Find a good solution for application settins. Maybe add EEPROM access to ApplicationInterface class
  // RSSIServer = new ApplicationRSSIMeasServer(440);
  // RSSIClient = new ApplicationRSSIMeasClient(Config::get()->getWSO_HOST(), 440);
}

void loop()
{
  wdt_reset();
  // ESP.wdtFeed();

  ConnectionMgr::get()->loop();
  if (serial)
    serial->loop();

  if (RSSIServer)
    RSSIServer->loop();

  if (RSSIClient)
    RSSIClient->loop();
}
