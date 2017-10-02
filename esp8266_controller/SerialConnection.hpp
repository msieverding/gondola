#ifndef _SERIAL_CONNECTION_HPP_
#define _SERIAL_CONNECTION_HPP_

#include "Gondola.hpp"
#include "IConnection.hpp"
#include "CommandInterpreter.hpp"

/**
 * Handles the serial connection, which could be connected to a computer
 */
class SerialConnection : public IConnection
{
public:
  /**
   * Create the instance of the serial connection
   * @param  baudrate Baudrate to use
   * @param  gondola  Gondola to use for requests
   * @return          pointer to Instance
   */
  static SerialConnection *create(uint32_t baudrate);

  /**
   * Vrtual destructor
   */
  virtual ~SerialConnection();

  /**
   * Call this loop periodically to handle serial commands etc.
   */
  virtual void loop();

private:
  /**
   * Constructor
   * @param baudrate Baudrate to use for UART
   */
  SerialConnection(uint32_t baudrate);

  /**
   * CI Command to change log level
   * @param s Command to interprete
   */
  bool loglevelCommand(std::string &s);

  // Instance
  static SerialConnection    *s_Instance;           //!< instance of singleton
  // Membervariables
  uint32_t                    m_Baudrate;           //!< Baudrate of UART
  CommandInterpreter         *m_CommandInterpreter; //!< Pointer to CommandInterpreter
};

#endif /* _SERIAL_CONNECTION_HPP_ */
