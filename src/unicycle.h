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

  void applyWheelImpulse(double impulse);
  void applyForkImpulse(double impulse);

  void reset(btTransform const& t);

protected:
  void createForkShape(WorldManager& wm);
  void createWheelShape(WorldManager& wm);
  void createForkBody(WorldManager& wm, btTransform const& t);
  void createWheelBody(WorldManager& wm, btTransform const& t);

private:
  btScalar m_forkWidth;
  btScalar m_forkLength;
  btScalar m_wheelWidth;
  btScalar m_wheelRadius;

  btScalar m_forkMass;
  btScalar m_wheelMass;

  btCompoundShape* m_forkShape;
  btCylinderShapeZ* m_wheelShape;
  btRigidBody* m_forkBody;
  btRigidBody* m_wheelBody;
};

#endif // UNICYCLE_H