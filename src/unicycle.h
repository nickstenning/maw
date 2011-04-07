#ifndef UNICYCLE_H
#define UNICYCLE_H

#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btTransform.h>

class btRigidBody;
class WorldManager;
class btCompoundShape;
class btCylinderShapeZ;

class UnicycleAlreadyInitializedError {};

class Unicycle
{
public:

  static const btTransform reset_transform;

  Unicycle(
    btScalar fork_length,
    btScalar wheel_radius,
    btScalar drive_radius,
    btScalar rider_mass,
    btScalar wheel_mass,
    btScalar drive_mass,
    bool drive_limits
  );

  void add_to_manager(WorldManager& wm) throw(UnicycleAlreadyInitializedError);

  void apply_wheel_impulse(btScalar imp);
  void apply_drive_impulse(btScalar imp);

  void reset(bool randomize=false, btTransform const& t = Unicycle::reset_transform);
  void reset_position(bool randomize=false, btTransform const& t = Unicycle::reset_transform);

  void compute_state();

  btVector3 const& origin() const;
  btScalar yaw() const;
  btScalar pitch() const;
  btScalar roll() const;
  btScalar wheel_velocity() const;
  btScalar yaw_velocity() const;

protected:
  void create_collision_shapes(WorldManager& wm);
  void create_rigid_bodies(WorldManager& wm, btTransform const& t);

  btVector3 contact_point() const;
private:
  btScalar m_fork_length;
  btScalar m_wheel_radius;
  btScalar m_drive_radius;

  btScalar m_rider_mass;
  btScalar m_wheel_mass;
  btScalar m_drive_mass;

  btScalar m_yaw;
  btScalar m_pitch;
  btScalar m_roll;

  btScalar m_wheel_velocity;
  btScalar m_yaw_velocity;

  btCompoundShape* m_fork_shape;
  btCylinderShapeZ* m_wheel_shape;
  btCylinderShape* m_drive_shape;
  btRigidBody* m_fork_body;
  btRigidBody* m_wheel_body;
  btRigidBody* m_drive_body;

  bool m_drive_limits;
};

#endif // UNICYCLE_H
