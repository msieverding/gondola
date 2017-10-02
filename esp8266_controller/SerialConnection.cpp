#include "SerialConnection.hpp"
#include "Log.hpp"
#include "ConnectionMgr.hpp"
#include <string>

SerialConnection *SerialConnection::s_Instance = NULL;

SerialConnection *SerialConnection::create(uint32_t baudrate)
{
  if (!s_Instance)
    s_Instance = new SerialConnection(baudrate);
  return s_Instance;
}

SerialConnection::SerialConnection(uint32_t baudrate)
 : m_Baudrate(baudrate)
 , m_CommandInterpreter()
{
  Serial.begin(m_Baudrate);
  Serial.println("\n\n");
  logDebug("Starting Serial Connection\n");

  m_CommandInterpreter = CommandInterpreter::get();
  m_CommandInterpreter->addCommand("loglevel", std::bind(&SerialConnection::loglevelCommand, this, std::placeholders::_1));

}

SerialConnection::~SerialConnection()
{
  m_CommandInterpreter->deleteCommand("loglevel");
  delete(m_CommandInterpreter);
  s_Instance = NULL;
}

void SerialConnection::loop()
{
  if (Serial.available() > 0)
  {
    char command[255];
    uint8_t bytesRead;

    // read a line from serial
    bytesRead = Serial.readBytesUntil('\n', command, 255);

    // Set a '\0' at the end as a char-string terminator
    if (command[bytesRead - 1] == '\r')   // check if CR was send
    {
      command[bytesRead - 1] = '\0';      // then overwrite it with '\0'
    }
    else
    {
      bytesRead++;
      command[bytesRead - 1] = '\0';      // Set'\0' as last charachter
    }

    std::string cmd(command);             // make a string out of it

    m_CommandInterpreter->interprete(cmd);// interprete the string
  }
}

bool SerialConnection::loglevelCommand(std::string &s)
{
  CommandInterpreter *CI = CommandInterpreter::get();
  std::string arg0;
  CI->getArgument(s, arg0, 0);
  if (CI->getNumArgument(s) == 1)
  {
    if (arg0.compare("info") == 0)
    {
      setLogLevel(LOG_INFO);
      return true;
    }
    else if (arg0.compare("debug") == 0)
    {
      setLogLevel(LOG_DEBUG);
      return true;
    }
    else if (arg0.compare("warning") == 0)
    {
      setLogLevel(LOG_WARNING);
      return true;
    }
    else if (arg0.compare("verbose") == 0)
    {
      setLogLevel(LOG_VERBOSE);
      Config::get()->writeToEEPROM();
      return true;
    }
  }

  logWarning("Unsupported!\n");
  logWarning("Usage: loglevel level\nLevels:\n\twarning\tonly warnings are displayed\n\tinfo\tadditional information is displayed\n\tdebug\taddtitional debug output is provided\n\tverbose\tall generated output will be printed\n");
  return false;
}
