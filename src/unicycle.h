#ifndef UNICYCLE_H
#define UNICYCLE_H

#include <LinearMath/btScalar.h>
#include <LinearMath/btTransform.h>

class btRigidBody;
class WorldManager;

class Unicycle
{
public:

  static const btTransform resetTransform;

  Unicycle();

  void addToManager(WorldManager& wm);

  void applyWheelImpulse(double impulse);
  void applyForkImpulse(double impulse);

  void reset(btTransform const& t = Unicycle::resetTransform);
  void translate(btScalar x, btScalar y, btScalar z);

  void updateAngles();

  btScalar yaw() const;
  btScalar pitch() const;
  btScalar roll() const;

  btTransform transform();
  Unicycle& transform(btTransform const&);

protected:
  void createForkShape(WorldManager& wm);
  void createWheelShape(WorldManager& wm);
  void createForkBody(WorldManager& wm, btTransform const& t);
  void createWheelBody(WorldManager& wm, btTransform const& t);

  void setEuler(btScalar y, btScalar p, btScalar r);

private:
  btScalar m_forkWidth;
  btScalar m_forkLength;
  btScalar m_wheelWidth;
  btScalar m_wheelRadius;

  btScalar m_forkMass;
  btScalar m_wheelMass;

  btScalar m_yaw;
  btScalar m_pitch;
  btScalar m_roll;

  btTransform m_transform;

  btCompoundShape* m_forkShape;
  btCylinderShapeZ* m_wheelShape;
  btRigidBody* m_forkBody;
  btRigidBody* m_wheelBody;
};

#endif // UNICYCLE_H