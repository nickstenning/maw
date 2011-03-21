#ifndef UNICYCLE_H
#define UNICYCLE_H

#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btTransform.h>

class btRigidBody;
class WorldManager;
class btCompoundShape;
class btCylinderShapeZ;

class Unicycle
{
public:

  static const btTransform reset_transform;

  Unicycle(
    btScalar fork_width,
    btScalar fork_length,
    btScalar wheel_width,
    btScalar wheel_radius,
    btScalar seat_mass,
    btScalar wheel_mass
  );

  void add_to_manager(WorldManager& wm);

  void apply_wheel_impulse(btScalar impulse);
  void apply_fork_impulse(btScalar impulse);

  void reset(btTransform const& t = Unicycle::reset_transform);
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
  void create_collision_shapes(WorldManager& wm);
  void create_rigid_bodies(WorldManager& wm, btTransform const& t);

  btVector3 contact_point();

private:
  btScalar m_fork_width;
  btScalar m_fork_length;
  btScalar m_wheel_width;
  btScalar m_wheel_radius;

  btScalar m_seat_mass;
  btScalar m_wheel_mass;

  btScalar m_yaw;
  btScalar m_pitch;
  btScalar m_roll;

  btScalar m_wheel_velocity;
  btScalar m_yaw_velocity;

  btTransform m_transform;

  btCompoundShape* m_fork_shape;
  btCylinderShapeZ* m_wheel_shape;
  btRigidBody* m_fork_body;
  btRigidBody* m_wheel_body;
};

#endif // UNICYCLE_H
