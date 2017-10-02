#ifndef _I_CONNECTION_HPP_
#define _I_CONNECTION_HPP_

/**
 * Interface for connections.
 * Used for WiFiConnection, APConnection, DualConnection and SerialConnection for polymorphie
 */
class IConnection
{
public:
  /**
   * Loop to call periodically
   */
  virtual void loop() {};

  /**
   * Virtual destrcutor
   */
  virtual ~IConnection() {}

protected:
  /**
   * Constrcutor
   */
  IConnection() {}
};

#endif /* _I_CONNECTION_HPP_ */
