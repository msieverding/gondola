#ifndef _APPLICATION_INTERFACE_HPP_
#define _APPLICATION_INTERFACE_HPP_

#include <Arduino.h>
#include "Gondola.hpp"
#include "WebServer.hpp"

/**
 * Interface for an application. Gondola will register here and any application
 * has access to it
 */
class ApplicationInterface
{
public:
  /**
   * get the instance of this interface
   * @return instance of the interface
   */
  static ApplicationInterface* get();

  /**
   * Virtual destructor
   */
  virtual ~ApplicationInterface();

  /**
   * regiter a gondola to the webserer
   * @param gondola pointer to gondola
   */
  void registerGondola(Gondola *gondola);

  /**
   * Get the regitered Gondola
   * @return reference to Gondola
   */
  Gondola* getGondola();

private:
  /**
   * Private constructor
   */
  ApplicationInterface();

  // Instance
  static ApplicationInterface       *s_Instance;        //!< static isntance

  // Membervariables
  Gondola                           *m_Gondola;         //!< pointer registered Gondol
};
#endif /* _APPLICATION_INTERFACE_HPP_ */
