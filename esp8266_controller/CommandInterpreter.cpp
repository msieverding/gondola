#include "CommandInterpreter.hpp"
#include "Log.hpp"
#include "Config.hpp"
#include <Arduino.h>
#include <string.h>
#include "ConnectionMgr.hpp"

CommandInterpreter *CommandInterpreter::s_Instance = NULL;

CommandInterpreter *CommandInterpreter::get()
{
  if (!s_Instance)
  {
    s_Instance = new CommandInterpreter();
  }
  return s_Instance;
}

CommandInterpreter::CommandInterpreter()
 : m_CommandList()
{
  addCommand("help", std::bind(&CommandInterpreter::helpCommand, this, std::placeholders::_1));
  addCommand("configReset", std::bind(&CommandInterpreter::configResetCommand, this, std::placeholders::_1));
  addCommand("configPrint", std::bind(&CommandInterpreter::configPrintCommand, this, std::placeholders::_1));
}

CommandInterpreter::~CommandInterpreter()
{
  deleteCommand("help");
  deleteCommand("configReset");
  deleteCommand("configPrint");
  s_Instance = NULL;
}

void CommandInterpreter::addCommand(std::string commandWord, commandFunc commandFunction)
{
  m_CommandList.push_back({commandWord, commandFunction});
}

void CommandInterpreter::deleteCommand(std::string commandWord)
{
  std::list<command_t>::iterator it = m_CommandList.begin();
  while (it != m_CommandList.end())
  {
    if (it->command.compare(commandWord) == 0)
    {
      // Increment first and than delete old one:
      // https://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
      m_CommandList.erase(it++);
    }
    else
    {
      it++;
    }
  }
}

void CommandInterpreter::interprete(std::string &s)
{
  bool done = false;
  std::string commandWord = getCommandWord(s);

  for (std::list<command_t>::iterator it = m_CommandList.begin(); it != m_CommandList.end(); it++)
  {
    if (it->command.compare(commandWord) == 0)
    {
      if (it->func)
      {
        bool ret;
        ret = it->func(s);
        if (ret == false)
          logWarning("CI: Fail.\n");
        else
          logInfo("CI: Success.\n");
        done = true;
        // Don't brake here
        // Maybe there is more than one function inside the list!
      }
    }
  }
  if (!done)
  {
    if (commandWord.length()) // ignore simple 'Return' presses
      logInfo("Command '%s' not registered. Try help for a list of registered commands\n", s.c_str());
  }
}

std::string CommandInterpreter::getCommandWord(std::string &s)
{
  String arduinoS(s.c_str());
  const char token = ' ';

  // unfortunatly string::find does not compile/link under arduino
  // workaround: use arduino String and indexOf
  int32_t pos = arduinoS.indexOf(token);
  if (pos >= 0)
  {
    return s.substr(0, pos);
  }
  // if no token is present. take wohle input
  return s;
}

bool CommandInterpreter::getArgument(std::string &s, std::string &arg, uint8_t argNum)
{
  String arduinoS(s.c_str());
  const char token = ' ';

  int32_t posBefore = 0, posBehind = 0;

  for (uint8_t i = 0; i <= argNum; i++)
  {
    posBefore = arduinoS.indexOf(token, posBehind);
    if (posBefore == -1)
      return false;
    posBehind = arduinoS.indexOf(token, posBefore + 1);
    if (posBehind == -1)
      posBehind = arduinoS.length();
  }

  arg = s.substr(posBefore + 1, posBehind - posBefore - 1);

  return true;
}

uint8_t CommandInterpreter::getNumArgument(std::string &s)
{
  String arduinoS(s.c_str());
  const char token = ' ';
  int8_t pos = 0, oldPos = 0;
  uint8_t args = 0;

  while(1)
  {
    pos = arduinoS.indexOf(token, oldPos);
    if (pos == -1)
      return args;

    args++;
    oldPos = pos + 1;
  }
}

bool CommandInterpreter::helpCommand(std::string &s)
{
  logInfo("Registered commands:\n");
  for (std::list<command_t>::iterator it = m_CommandList.begin(); it != m_CommandList.end(); it++)
  {
    logInfo("%s\n", it->command.c_str());
  }
  return true;
}

bool CommandInterpreter::configResetCommand(std::string &s)
{
  logDebug("CI: configResetCommand\n");
  Config::resetConfig();
  logWarning("ConnectionMgr will reset now!\n");
  ConnectionMgr::reset();
  logWarning("Done.\n");
  return true;
}

bool CommandInterpreter::configPrintCommand(std::string &s)
{
  logDebug("CI: configResetPrint\n");
  Config::get()->printConfig();
  return true;
}
