#include "IAnchor.hpp"
#include "Log.hpp"

IAnchor::IAnchor(uint8_t id, Coordinate anchorPosition, float spooledDistance, float targetSpooledDistance, float ropeOffset)
 : m_ID(id)
 , m_AnchorPosition(anchorPosition)
 , m_SpooledDistance(spooledDistance)
 , m_TargetSpooledDistance(targetSpooledDistance)
 , m_InitCallback()
 , m_ReadyCallback()
 , m_TravelTime(0)
 , m_RopeOffset(ropeOffset)
{

}

IAnchor::~IAnchor()
{

}

bool IAnchor::setInitialSpooledDistance(float spooledDistance)
{
  m_SpooledDistance = spooledDistance;
  m_TargetSpooledDistance = m_SpooledDistance;
  logDebug("Anchor '%d': Incoming initial spooling '%s'.\n", getID(), FTOS(m_SpooledDistance));
  return executeInitCallback();
}

Coordinate IAnchor::getAnchorPos()
{
  return m_AnchorPosition;
}

void IAnchor::setAnchorPos(Coordinate anchorPos)
{
  m_AnchorPosition = anchorPos;
}

void IAnchor::registerInitCallback(callback cb)
{
  m_InitCallback = cb;
}

bool IAnchor::executeInitCallback()
{
  if (m_InitCallback)
    return m_InitCallback(this);
  return true;
}

void IAnchor::registerReadyCallback(callback cb)
{
  m_ReadyCallback = cb;
}

bool IAnchor::executeReadyCallback()
{
  logDebug("IAnchor::executeReadyCallback\n");
  if (m_ReadyCallback)
    return m_ReadyCallback(this);
  return true;
}

uint8_t IAnchor::getID()
{
  return m_ID;
}

float IAnchor::getSpooledDistance()
{
  return m_SpooledDistance;
}

float IAnchor::getTargetSpooledDistance()
{
  return m_TargetSpooledDistance;
}

uint32_t IAnchor::getTravelTime()
{
  return m_TravelTime;
}

float IAnchor::getRopeOffset()
{
  return m_RopeOffset;
}

void IAnchor::setRopeOffset(float offset)
{
  m_RopeOffset = offset;
}

float IAnchor::roundPrecision(float f, float precision)
{
  return round(f * (1.0f / precision)) / (1.0f / precision);
}
