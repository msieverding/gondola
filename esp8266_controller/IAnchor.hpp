#ifndef _I_ANCHOR_HPP_
#define _I_ANCHOR_HPP_

#include "Coordinate.hpp"
#include "Config.hpp"
#include <functional>
#include <Ticker.h>

/**
 * Class for an Anchor of a gondola
 */
class IAnchor
{
public:
  /**
   * Type for the callback, that should be executed, when movement is finished
   */
  typedef std::function<bool(IAnchor *)> callback;

  /**
   * Constructor
   * Values can be changed after construction.
   * @param id                    id of the anchor
   * @param anchorPosition        position of the anchor
   * @param spooledDistance       current spooled distance of the anchor
   * @param targetSpooledDistance target spooled distance of the anchor
   * @param ropeOffset            offset between motor and anchor in cm
   */
  IAnchor(uint8_t id, Coordinate anchorPosition, float spooledDistance, float targetSpooledDistance, float ropeOffset);

  /**
   * vitual Destructor
   */
  virtual ~IAnchor();

  /**
   * Set the mounting position of the anchor
   * @param anchorPos mounting position
   */
  void setAnchorPos(Coordinate anchorPos);

  /**
   * @return Coordinate, where the anchor is mounted
   * Get the mounting position of the anchor
   */
  Coordinate getAnchorPos();

  /**
   * Get the current spooled distance
   * @return spooled distance
   */
  float getSpooledDistance();

  /**
   * Get the target spooled distance
   * @return target spooled distance
   */
  float getTargetSpooledDistance();

  /**
   * Get the time budget for the current movement
   * @return travel time
   */
  uint32_t getTravelTime();

  /**
   * Set the spooled distance initially
   * @param spooledDistance spooled distance to set
   */
  virtual bool setInitialSpooledDistance(float spooledDistance);

  /**
   * Set the new spooling targetDistance.
   * @param targetDistance Distance to spool to
   * @return               steps needed for movement
   */
  virtual uint32_t setTargetSpooledDistance(float targetDistance) = 0;

  /**
   * Start the movement.
   * target spooled distance must be set with setTargetSpooledDistance before to
   * setup the movement
   * @param  traveltime time budget for the movement
   * @return            success. Could be false if communication fails
   */
  virtual bool startMovement(uint32_t traveltime) = 0;

  /**
   * Register a callback, that should be executed, when an initial spooled distance should be set
   * @param cb callback to execute
   */
  void registerInitCallback(callback cb);

  /**
   * Execute the initCallback if registered
   * @return return of init callback
   */
  virtual bool executeInitCallback();

  /**
   * Register a callback, that should be executed, when the movement is finished
   * @param cb callback to execute
   */
  void registerReadyCallback(callback cb);

  /**
   * Execute the readyCallback if registered
   * @return return of readyCallback
   */
  virtual bool executeReadyCallback();

  /**
   * Loop that should be executed with high frequency in the main arduino loop
   *
   * Makes the movemetn of the stepper motor when requestes
   */
  virtual void loop() = 0;

  /**
   * Get the ID of the anchor
   * @return Id of this anchor
   */
  uint8_t getID();

  /**
   * Get the rope offset of the anchor
   * @return rope offset
   */
  float getRopeOffset();

  /**
   * Set the rope offset of the anchor
   * @param offset offset in cm
   */
  void setRopeOffset(float offset);

  /**
   * Round a float to a given precision
   * @param f             float to round
   * @param precision     precision of rounding
   * @return  rounded f
  */
  static float roundPrecision(float f, float precision);


protected:

  // Membervariables
  uint8_t           m_ID;                         //!< ID of Anchor. Could be used for a protocoll above
  Coordinate        m_AnchorPosition;             //!< Mounting position
  float             m_SpooledDistance;            //!< Current spooled distance
  float             m_TargetSpooledDistance;      //!< Target spooled distance
  callback          m_InitCallback;               //!< Callback to set an initial spooled distance
  callback          m_ReadyCallback;              //!< Callback that should be executed to propagate the finished movement
  uint32_t          m_TravelTime;                 //!< Time budget for movement
  float             m_RopeOffset;                 //!< Spool that is spooled from motor to anchor
};

#endif /* _I_ANCHOR_HPP_ */
