#include "HardwareAnchor.hpp"
#include "Log.hpp"

HardwareAnchor *s_MoveInstance = NULL;

HardwareAnchor::HardwareAnchor(uint8_t id)
 : IAnchor(id, Config::get()->getGO_ANCHORPOS(), 0.0f , 0.0f, Config::get()->getGO_ROPEOFFSET())
 , m_Pins({0, 5, 4})
 , m_StepsTodo(0)
 , m_StepsDone(0)
 , m_Direction(1)
 , m_MoveStartTime(0)
 , m_MovementFinished(false)
 , m_Timer()
{
  logDebug("Creating HardwareAnchor at (%s) with a spooled offset of (%s)\n", m_AnchorPosition.toString().c_str(), FTOS(m_RopeOffset));
  configurePins();
  s_MoveInstance = this;
}

HardwareAnchor::~HardwareAnchor()
{
  s_MoveInstance = NULL;
}

uint32_t HardwareAnchor::setTargetSpooledDistance(float targetDistance)
{
  float distanceTodo = targetDistance - m_SpooledDistance;
  distanceTodo = roundPrecision(distanceTodo, MIN_PRECISION);   // round to a given precision (1 step)
  m_TargetSpooledDistance = m_SpooledDistance + distanceTodo;

  logVerbose("============ Anchor Computing stuff ==================\n");
  logVerbose("Anchor ID: %d on position: %s\n", m_ID, m_AnchorPosition.toString().c_str());
  logVerbose("Spooled: %scm, Delta: %scm\n", FTOS(m_SpooledDistance), FTOS(distanceTodo));

  if (distanceTodo < 0)
  {
    digitalWrite(m_Pins.dir, LOW);
    m_Direction = -1;
    distanceTodo = abs(distanceTodo);
  }
  else
  {
    digitalWrite(m_Pins.dir, HIGH);
    m_Direction = 1;
  }
  // calculate number of steps todo
  m_StepsTodo = distanceTodo * STEP_CM;

  logVerbose("Rounded to: (%scm): %scm, steps: %ld, microsteps: %ld\n", floatToString(MIN_PRECISION).c_str(), floatToString(distanceTodo).c_str(), m_StepsTodo, m_StepsTodo * MICROSTEPS);

  m_StepsTodo *= MICROSTEPS; // we need to account for all microsteps
  m_StepsDone = 0;

  logVerbose("======================================================\n");

  return m_StepsTodo;
}

bool HardwareAnchor::startMovement(uint32_t traveltime)
{
  m_TravelTime = traveltime;
  logDebug("Start: %d\n", millis());
  m_MoveStartTime = millis();
  m_Timer.attach_ms(1, move);
  return true;
}

void HardwareAnchor::configurePins()
{
  pinMode(m_Pins.en, OUTPUT);
  pinMode(m_Pins.stp, OUTPUT);
  pinMode(m_Pins.dir, OUTPUT);
  digitalWrite(m_Pins.en, LOW);
}

inline void HardwareAnchor::startStep()
{
  digitalWrite(m_Pins.stp, HIGH);   // start stepp trigger
}

inline void HardwareAnchor::endStep()
{
  digitalWrite(m_Pins.stp, LOW);    // stop step trigger
}

void HardwareAnchor::move()
{
  uint32_t stepsGoal = ceil(((float)((millis() - s_MoveInstance->m_MoveStartTime) * s_MoveInstance->m_StepsTodo)) / s_MoveInstance->m_TravelTime);
  if (stepsGoal > s_MoveInstance->m_StepsTodo)
    stepsGoal = s_MoveInstance->m_StepsTodo;

  while (s_MoveInstance->m_StepsDone < stepsGoal)
  {
    s_MoveInstance->startStep();
    delayMicroseconds(10); // delay to be sure that the step was done
    s_MoveInstance->endStep();
    delayMicroseconds(10); // delay to be sure that the step was done
    s_MoveInstance->m_SpooledDistance += s_MoveInstance->m_Direction * (1.0f / STEP_CM / MICROSTEPS);
    s_MoveInstance->m_StepsDone++;
  }

  if (s_MoveInstance->m_StepsDone == s_MoveInstance->m_StepsTodo)
  {
    s_MoveInstance->m_MovementFinished = true;
    s_MoveInstance->m_Timer.detach();
  }
}

void HardwareAnchor::loop()
{
  if (m_MovementFinished == true)
  {
    m_MovementFinished = false;
    m_MoveStartTime = 0;
    executeReadyCallback();
    logDebug("Finish: %d, m_SpooledDistance=%s.\n", millis(), FTOS(m_SpooledDistance));
  }
}
