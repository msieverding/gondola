#ifndef _HARDWARE_ANCHOR_HPP_
#define _HARDWARE_ANCHOR_HPP_

#include "IAnchor.hpp"

/** Struct for pin setup of ESP8266 */
typedef struct {
  uint8_t en;     //!< enable pin
  uint8_t stp;    //!< step pin
  uint8_t dir;    //!< direction pin
} pins_t;

class HardwareAnchor : public IAnchor
{
public:

  /**
   * Constrcutor
   */
  HardwareAnchor(uint8_t id);

  /**
   * Destructor
   */
  ~HardwareAnchor();

  /**
   * Set the new spooling targetDistance.
   * Anchor will start to move with an ISR driven function that
   * toggles the pin for the stepper driver
   * @see move()
   * @param targetDistance Distance to spool to
   * @param travelTime     Time that should be needed
   */
  virtual uint32_t setTargetSpooledDistance(float targetDistance);

  /**
   * Start the movement.
   * Call setTargetSpooledDistance before to setup the movement
   * @param  travelTime time budget for travlling
   * @return            ever true
   */
  virtual bool startMovement(uint32_t travelTime);

  /**
   * Loop that should be executed with high frequency in the main arduino loop
   *
   * Makes the movemetn of the stepper motor when requestes
   */
  virtual void loop();

private:

  /**
   * Configure the hardware pins
   */
  void configurePins();

  /**
   * start a step with the hardware
   */
  void startStep();

  /**
   * end a step with the hardware
   */
  void endStep();

  /**
   * Callback for timer to toggle pins of stepper
   */
  static void move();

  // Membervariables
  pins_t            m_Pins;                       //!< pin setup
  uint32_t          m_StepsTodo;                  //!< necessary steps to reach the target position
  uint32_t          m_StepsDone;                  //!< steps already done
  int8_t            m_Direction;                  //!< Direction of movement -1 or +1
  uint32_t          m_MoveStartTime;              //!< Time where movement starts
  bool              m_MovementFinished;           //!< Flag that indicates that the movement is finished. @see move() @see loop()
  Ticker            m_Timer;                      //!< ISR based timer for movement
};

#endif /* _HARDWARE_ANCHOR_HPP_ */
