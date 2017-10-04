#include "ApplicationInterface.hpp"

ApplicationInterface* ApplicationInterface::s_Instance = NULL;

ApplicationInterface* ApplicationInterface::get()
{
  if (s_Instance == NULL)
    s_Instance = new ApplicationInterface();

  return s_Instance;
}

ApplicationInterface::ApplicationInterface()
 : m_Gondola(NULL)
{

}

ApplicationInterface::~ApplicationInterface()
{

}


void ApplicationInterface::registerGondola(Gondola *gondola)
{
  m_Gondola = gondola;
}

Gondola *ApplicationInterface::getGondola()
{
  return m_Gondola;
}
