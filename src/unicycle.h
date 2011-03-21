#ifndef UNICYCLE_H
#define UNICYCLE_H

#include <LinearMath/btScalar.h>
#include <LinearMath/btTransform.h>

#define YAW_BANG_SIZE 2.0
#define PITCH_BANG_SIZE 5.0

class btRigidBody;
class WorldManager;
class btCompoundShape;
class btCylinderShapeZ;

class Unicycle
{
public:

  static const btTransform resetTransform;

  Unicycle();

  void add_to_manager(WorldManager& wm);

  void apply_wheel_impulse(double impulse, bool force=false);
  void apply_fork_impulse(double impulse, bool force=false);

  void reset(btTransform const& t = Unicycle::resetTransform);
  void translate(btScalar x, btScalar y, btScalar z);

  void compute_state();

  btScalar yaw() const;
  btScalar pitch() const;
  btScalar roll() const;
  btScalar wheel_velocity() const;
  btScalar yaw_velocity() const;

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

  btScalar m_wheel_velocity;
  btScalar m_yaw_velocity;

  int m_yawRestore;

  btTransform m_transform;

  btCompoundShape* m_forkShape;
  btCylinderShapeZ* m_wheelShape;
  btRigidBody* m_forkBody;
  btRigidBody* m_wheelBody;
};

#endif // UNICYCLE_H