#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <string>
#include <list>

/**
 * Enumeration for the different log levels
 */
typedef enum logLevel_e {
  LOG_WARNING,        //!< Display only critical warnings
  LOG_INFO,           //!< Display also user information
  LOG_DEBUG,          //!< Display also debug ouput
  LOG_VERBOSE         //!< Display all possible output
} logLevel_t;

/**
 * Converter union for 4 byte types
 */
typedef union b4Converter_u {
  float f;        //!< float
  uint8_t b[4];   //!< bytewise array
  uint32_t u;     //!< uint32_t
} b4Converter_t;

/**
 * Set a log level
 * @param level level to set
 */
void setLogLevel(logLevel_t level);

/**
 * Ad a string to the logpage
 * @param s string to add
 */
void addLogPageString(std::string &s);

/**
 * Get the list of strings from the logpage
 * @return list of strings
 */
std::list<std::string> &getLogPageList();

/**
 * Log a warning message
 * @param format  printf format
 */
void logWarning(const char *format, ...);

/**
 * Log an info message
 * @param format  printf format
 */
void logInfo(const char *format, ...);

/**
 * Log a debug message
 * @param format  printf format
 */
void logDebug(const char *format, ...);

/**
 * Log a verbose message
 * @param format  printf format
 */
void logVerbose(const char *format, ...);

/**
 * Convert a float to a string. Unfortunatly Arduino framework
 * doesn't support %f in printf
 * @param  f float to convert
 * @return   string with float representation inside
 */
std::string floatToString(float f);

/**
 * Convert a string to a float.
 * E.g. from a JSON string
 * @param  s string to convert
 * @return   float value of string
 */
float stringToFloat(std::string s);

/**
 * Quick macro to use with %s instead of %f
 * @see floatToString()
 * @param  f f to print
 * @return   c-str with float inside
 */
#define FTOS(f) floatToString(f).c_str()

#endif /* _LOG_HPP_ */
