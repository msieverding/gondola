#include <Arduino.h>
#include "config.hpp"
#include "Log.hpp"
#include "Gondola.hpp"
#include "CommandInterpreter.hpp"

Gondola::Gondola()
 : m_CurrentPosition(Config::get()->getGO_POSITION())
 , m_TargetPosition(m_CurrentPosition)
 , m_AnchorList()
 , m_UnfinishedAnchors(0)
 , m_TravelTime(0)
{
  logDebug("Creating gondola at: %s\n", m_CurrentPosition.toString().c_str());
  CommandInterpreter::get()->addCommand("move", std::bind(&Gondola::moveCommand, this, std::placeholders::_1));
}

Gondola::~Gondola()
{
  CommandInterpreter::get()->deleteCommand("move");
  std::list<IAnchor *>::iterator it = m_AnchorList.begin();
  while (it != m_AnchorList.end())
  {
    logDebug("Delete Anchor %d from list\n", (*it)->getID());
    deleteAnchor(it++);
  }
}

void Gondola::setInitialPosition(Coordinate position)
{
  m_CurrentPosition = position;
  m_TargetPosition = position;
  std::list<IAnchor *>::iterator it = m_AnchorList.begin();
  while (it != m_AnchorList.end())
  {
    IAnchor *anchor = *it;
    if (anchor->getID() != HW_ANCHOR_ID)
    {
      float spooledDistance = calculateCorrectedSpooling(anchor, Coordinate::euclideanDistance(anchor->getAnchorPos(), m_CurrentPosition));
      if (anchor->setInitialSpooledDistance(spooledDistance) == false)
      {
        logWarning("Unable to init anchor %d. No Connection available. Delete it from Gondola.\n", anchor->getID());
        deleteAnchor(it++);
        continue;
      }
    }
    it++;
  }
}

void Gondola::addAnchor(IAnchor *anchor)
{
  float spooledDistance = calculateCorrectedSpooling(anchor, Coordinate::euclideanDistance(anchor->getAnchorPos(), m_CurrentPosition));
  anchor->setInitialSpooledDistance(spooledDistance);
  m_AnchorList.push_front(anchor);        // push new (remote) anchors to the front, so that the hardware anchor starts to spool after the message to the remote anchors was delivered (better synchronisation during spooling)
}

void Gondola::deleteAnchor(std::list<IAnchor *>::iterator it)
{
  IAnchor *anchor = *it;
  if (anchor->getID() != HW_ANCHOR_ID)
  {
    delete(anchor);
  }
  logDebug("Erase Anchor %d from list\n", anchor->getID());
  m_AnchorList.erase(it);
}

void Gondola::deleteAnchor(uint8_t id)
{
  std::list<IAnchor *>::iterator it = m_AnchorList.begin();
  while (it != m_AnchorList.end())
  {
    IAnchor *anchor = *it;
    if (anchor->getID() == id)
    {
      deleteAnchor(it++);
      continue;
    }
    it++;
  }
}

void Gondola::reportAnchorFinished(uint8_t id)
{
  logVerbose("Anchor '%d' finished moving\n", id);
  std::list<IAnchor *>::iterator it;
  for (it = m_AnchorList.begin(); it != m_AnchorList.end(); it++)
  {
    IAnchor *anchor = *it;
    if (anchor->getID() == id)
    {
      // TODO why atomic?
      noInterrupts();
      m_UnfinishedAnchors &= ~(1 << anchor->getID());
      interrupts();
    }
  }
  checkForReady();
}

Coordinate Gondola::getCurrentPosition()
{
  return m_CurrentPosition;
}

Coordinate Gondola::getTargetPosition()
{
  return m_TargetPosition;
}

uint32_t Gondola::getTravelTime()
{
  return m_TravelTime;
}

void Gondola::setTargetPosition(Coordinate &targetPos, float &speed)
{
  m_TargetPosition = targetPos;
  if (speed == 0.0f)
    speed = 1.0f;

  float travelDistance = Coordinate::euclideanDistance(m_CurrentPosition, m_TargetPosition);
  if (travelDistance == 0)
  {
    logDebug("Travel distance = 0. Nothing to do.\n");
    return;
  }

  logVerbose("============= Gondola Computing all Anchors ==========\n");

  float travelTime = travelDistance / speed;
  logVerbose("TravelDistance: %s, TravelTime: %s\n", FTOS(travelDistance), FTOS(travelTime));
  uint32_t maxSteps = 0;

  // prepare to spool
  std::list<IAnchor *>::iterator it;
  for (it = m_AnchorList.begin(); it != m_AnchorList.end(); it++)
  {
    uint32_t stepsTodo;
    IAnchor *anchor = *it;
    float targetSpooledDistance = Coordinate::euclideanDistance(anchor->getAnchorPos(), targetPos);

    float correctedSpooledDistance = calculateCorrectedSpooling(anchor, targetSpooledDistance);
    stepsTodo = anchor->setTargetSpooledDistance(correctedSpooledDistance);
    logVerbose("targetSpooledDistance: %s, correctedSpooledDistance: %s\n", FTOS(targetSpooledDistance), FTOS(correctedSpooledDistance));

    maxSteps = std::max(maxSteps, stepsTodo);
  }

  // TODO change value of 1000.0f to a realistic one
  logVerbose("Budget: %ss, Minimum %ss\n", FTOS(travelTime), FTOS(maxSteps / 1000.0f));

  logVerbose("======================================================\n");

  travelTime = std::max(travelTime, maxSteps / 1000.0f);
  travelTime *= 1000;
  m_TravelTime = static_cast<uint32_t>(travelTime);

  it = m_AnchorList.begin();
  while (it != m_AnchorList.end())
  {
    IAnchor *anchor = *it;
    m_UnfinishedAnchors |= (1 << anchor->getID());
    if (anchor->startMovement(m_TravelTime) == false)
    {
      logWarning("No connection to anchor %d possible. Delete anchor from list.\n", anchor->getID());
      deleteAnchor(it++);
      continue;
    }
    it++;
  }
}

std::list<IAnchor *> Gondola::getAnchorList(void)
{
  return m_AnchorList;
}

bool Gondola::moveCommand(std::string &s)
{
  CommandInterpreter *CI = CommandInterpreter::get();
  uint8_t args = CI->getNumArgument(s);
  Coordinate newPosition;
  float speed;

  if(args != 4)
  {
    logWarning("Unsupported!\n");
    logWarning("Usage: move x y z s\n");
    logWarning("\tx - float for x coordinate (e.g. 1.0)\n");
    logWarning("\ty - float for y coordinate (e.g. 1.0)\n");
    logWarning("\tz - float for z coordinate (e.g. 1.0)\n");
    logWarning("\ts - float for speed (e.g. 1.0)\n");
    return false;
  }
  std::string arg;
  CI->getArgument(s, arg, 0);
  newPosition.x = atof(arg.c_str());
  CI->getArgument(s, arg, 1);
  newPosition.y = atof(arg.c_str());
  CI->getArgument(s, arg, 2);
  newPosition.z = atof(arg.c_str());
  CI->getArgument(s, arg, 3);
  speed = atof(arg.c_str());

  setTargetPosition(newPosition, speed);
  return true;
}

void Gondola::checkForReady()
{
  if (m_UnfinishedAnchors == 0)
  {
    m_CurrentPosition = m_TargetPosition;
    Config::get()->setGO_POSITION(m_CurrentPosition);
    Config::get()->writeGOToEEPROM(true);
  }
}

float Gondola::calculateCorrectedSpooling(IAnchor *anchor, float targetSpooledDistance)
{
  // Err = b0 * realSpooledRope^2 + b1 * realSpooledRope + b2
  float b0 = -0.000024216423411;
  float b1 = 0.051005307386112;
  float b2 = 0.472932330827063;
  float ropeOffset = anchor->getRopeOffset();
  // Error at coordinate zero
  float estimatedErrZero = b0 * ropeOffset * ropeOffset + b1 * ropeOffset + b2;
  float ropeSpooledAtTarget = ropeOffset + targetSpooledDistance;
  // Error at target coordinate
  float estimatedErrTarget = b0 * ropeSpooledAtTarget * ropeSpooledAtTarget + b1 * ropeSpooledAtTarget + b2;
  // Estimated Error in movement
  float estimatedErr = estimatedErrTarget - estimatedErrZero;
  float correctedDistance = targetSpooledDistance - estimatedErr;

  logVerbose("Err estimation: estimatedErrZero: %s, estimatedErrTarget %s, estimatedErr %s\n", FTOS(estimatedErrZero), FTOS(estimatedErrTarget), FTOS(estimatedErr));
  logVerbose("Err estimation: targetSpooledDistance: %s, correctedDistance: %s\n", FTOS(targetSpooledDistance), FTOS(correctedDistance));
  return correctedDistance;
}

IAnchor *Gondola::getAnchor(uint8_t id)
{
  std::list<IAnchor *>::iterator it = m_AnchorList.begin();
  while (it != m_AnchorList.end())
  {
    IAnchor *anchor = *it;
    if (anchor->getID() == id)
    {
      return anchor;
    }
    it++;
  }
  return NULL;
}
