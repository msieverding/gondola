#ifndef _APPLICATION_INTERFACE_HPP_
#define _APPLICATION_INTERFACE_HPP_

#include <Arduino.h>
#include "Gondola.hpp"

// TODO Doc
class ApplicationInterface
{
public:
  static ApplicationInterface* get();
  ~ApplicationInterface();

  /**
   * regiter a gondola to the webserer
   * @param gondola pointer to gondola
   */
  void registerGondola(Gondola *gondola);
  Gondola* getGondola();

private:
  ApplicationInterface();

  static ApplicationInterface       *s_Instance;
  Gondola                           *m_Gondola;
};
#endif
