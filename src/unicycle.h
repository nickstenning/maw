#ifndef UNICYCLE_H
#define UNICYCLE_H

#include <LinearMath/btScalar.h>

class btTransform;
class btRigidBody;
class WorldManager;

class Unicycle
{
public:
  Unicycle();

  void addToManager(WorldManager& wm, btTransform const& t);
protected:
  void createForkShape(WorldManager& wm);
  void createWheelShape(WorldManager& wm);

private:
  btScalar m_forkWidth;
  btScalar m_forkLength;
  btScalar m_wheelWidth;
  btScalar m_wheelRadius;

  int m_forkIdx;
  int m_wheelIdx;

  btRigidBody* m_forkBody;
  btRigidBody* m_wheelBody;
};

#endif // UNICYCLE_H