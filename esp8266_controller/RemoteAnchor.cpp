#include "RemoteAnchor.hpp"
#include "Log.hpp"
#include <functional>

RemoteAnchor::RemoteAnchor(uint8_t id)
 : IAnchor(id, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f)
 , m_MoveCallback()
{

}

RemoteAnchor::~RemoteAnchor()
{

}

uint32_t RemoteAnchor::setTargetSpooledDistance(float targetDistance)
{
  /*
   * Just calculate how many step the ancor has todo, since gondola
   * needs the nubmer of steps to calculate smooth movement
   */
  float distanceTodo = targetDistance - m_SpooledDistance;
  distanceTodo = roundPrecision(distanceTodo, MIN_PRECISION);   // round to a given precision (1 step)
  m_TargetSpooledDistance = m_SpooledDistance + distanceTodo;

  if (distanceTodo < 0)
    distanceTodo = abs(distanceTodo);
    
  logVerbose("\nAnchor ID: %d on position: %s\n", m_ID, m_AnchorPosition.toString().c_str());
  logVerbose("Spooled: %scm, Delta: %scm\n", FTOS(m_SpooledDistance), FTOS(distanceTodo));

  // calculate number of steps todo
  uint32_t stepsTodo = distanceTodo * STEP_CM;

  logVerbose("Rounded to: (%scm): %scm, steps: %ld, microsteps: %ld\n\n", floatToString(MIN_PRECISION).c_str(), floatToString(distanceTodo).c_str(), stepsTodo, stepsTodo * MICROSTEPS);

  stepsTodo *= MICROSTEPS; // we need to account for all microsteps

  return stepsTodo;
}

bool RemoteAnchor::startMovement(uint32_t traveltime)
{
  m_TravelTime = traveltime;
  return m_MoveCallback(this);
}

void RemoteAnchor::loop()
{

}

void RemoteAnchor::registerMoveCallback(callback cb)
{
  m_MoveCallback = cb;
}

bool RemoteAnchor::executeMoveCallback()
{
  if (m_MoveCallback)
    return m_MoveCallback(this);
  return false;
}

bool RemoteAnchor::executeReadyCallback()
{
  m_SpooledDistance = m_TargetSpooledDistance;
  if (m_ReadyCallback)
    return m_ReadyCallback(this);
  return false;
}
