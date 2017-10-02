#include <Arduino.h>
#include <stdarg.h>
#include "Log.hpp"
#include "Config.hpp"
#include "ConnectionMgr.hpp"

#define MAX_LENGTH    256
#define LOG_LENGTH    20

static logLevel_t s_LogLevel = LOG_VERBOSE;
std::list<std::string> s_LogList(20);

void setLogLevel(logLevel_t level)
{
  s_LogLevel = level;
  Config::get()->setLOG_LEVEL(level);
  Config::get()->writeToEEPROM();
}

void addLogPageString(std::string &s)
{
  std::string tmp;
  uint16_t pos = 0;
  for (uint16_t i = 0; i < s.length(); i++)
  {
    if (s[i] == '\n')
    {
      tmp.append(s.substr(pos, i - pos));
      pos = i + 1;
      tmp.append("<br>");
    }
  }
  tmp.append(s.substr(pos));

  s_LogList.pop_front();

  if (tmp.length())
    s_LogList.push_back(tmp);
  else
    s_LogList.push_back(s);
}

std::list<std::string> &getLogPageList()
{
  return s_LogList;
}

void logDebug(const char *format, ...)
{
  if (s_LogLevel >= LOG_DEBUG)
  {
    char buf[MAX_LENGTH]; // resulting string limited to MAX_LENGTH chars
    va_list args;
    va_start (args, format );
    vsnprintf(buf, MAX_LENGTH, format, args);
    va_end (args);

    Serial.print("[DBG] ");
    Serial.print(buf);

    std::string bufs("[DBG] ");
    bufs.append(buf);
    addLogPageString(bufs);
  }
}

void logWarning(const char *format, ...)
{
  if (s_LogLevel >= LOG_WARNING)
  {
    char buf[MAX_LENGTH]; // resulting string limited to MAX_LENGTH chars
    va_list args;
    va_start (args, format );
    vsnprintf(buf, MAX_LENGTH, format, args);
    va_end (args);
    Serial.print("[WRN] ");
    Serial.print(buf);

    std::string bufs("<font color=\"red\">[WRN] ");
    bufs.append(buf);
    bufs.append("</font>");
    addLogPageString(bufs);
  }
}

void logInfo(const char *format, ...)
{
  if (s_LogLevel >= LOG_INFO)
  {
    char buf[MAX_LENGTH]; // resulting string limited to MAX_LENGTH chars
    va_list args;
    va_start (args, format );
    vsnprintf(buf, MAX_LENGTH, format, args);
    va_end (args);

    Serial.print("[INF] ");
    Serial.print(buf);

    std::string bufs("[INF] ");
    bufs.append(buf);
    addLogPageString(bufs);
  }
}

void logVerbose(const char *format, ...)
{
  if (s_LogLevel >= LOG_VERBOSE)
  {
    char buf[MAX_LENGTH]; // resulting string limited to MAX_LENGTH chars
    va_list args;
    va_start (args, format );
    vsnprintf(buf, MAX_LENGTH, format, args);
    va_end (args);

    Serial.print("[VBO] ");
    Serial.print(buf);

    std::string bufs("[VBO] ");
    bufs.append(buf);
    addLogPageString(bufs);
  }
}


std::string floatToString(float f)
{
  char buf[20];
  return std::string(dtostrf(f, 4, 2, buf));
}

float stringToFloat(std::string s)
{
  return (float)atof(s.c_str());
}
