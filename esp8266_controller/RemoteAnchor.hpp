#ifndef _REMOTE_ANCHOR_HPP_
#define _REMOTE_ANCHOR_HPP_

#include "IAnchor.hpp"

class RemoteAnchor : public IAnchor
{
public:
  RemoteAnchor(uint8_t id);
  ~RemoteAnchor();

  virtual uint32_t setTargetSpooledDistance(float targetDistance);
  virtual bool startMovement(uint32_t traveltime);

  virtual void loop();

  void registerMoveCallback(callback cb);
  bool executeMoveCallback();

  virtual bool executeReadyCallback();


private:

  callback        m_MoveCallback;
};

#endif /* _REMOTE_ANCHOR_HPP_ */
