#include <stdexcept>
#include <iostream>
#include <cmath>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"
#include "util.h"

const btTransform Unicycle::reset_transform = btTransform(btQuaternion::getIdentity());

const btScalar Unicycle::fric_static = 4.0;
const btScalar Unicycle::fric_kinetic = 1.0;

Unicycle::Unicycle(
  btScalar fork_length,
  btScalar wheel_radius,
  btScalar drive_radius,
  btScalar rider_mass,
  btScalar wheel_mass,
  btScalar drive_mass,
  bool drive_limits
) : m_fork_length(fork_length)
  , m_wheel_radius(wheel_radius)
  , m_drive_radius(drive_radius)
  , m_rider_mass(rider_mass)
  , m_wheel_mass(wheel_mass)
  , m_drive_mass(drive_mass)
  , m_yaw(0)
  , m_pitch(0)
  , m_roll(0)
  , m_yaw_velocity(0)
  , m_roll_velocity(0)
  , m_pitch_velocity(0)
  , m_wheel_velocity(0)
  , m_fork_shape(NULL)
  , m_wheel_shape(NULL)
  , m_drive_shape(NULL)
  , m_fork_body(NULL)
  , m_wheel_body(NULL)
  , m_drive_body(NULL)
  , m_drive_limits(drive_limits)
{}

void Unicycle::create_collision_shapes(WorldManager& wm)
{
  btScalar wheel_width = m_wheel_radius / 10.0;
  btScalar fork_width = m_fork_length / 20.0;

  // Make the unicycle fork
  {
    m_fork_shape = new btCompoundShape();

    float total_fork_width = 2.0 * (fork_width + wheel_width);
    btCollisionShape* leg = new btBoxShape(btVector3(fork_width / 2.0, m_fork_length / 2.0, fork_width / 2.0));
    btCollisionShape* top = new btBoxShape(btVector3(fork_width / 2.0, fork_width / 2.0, total_fork_width / 2.0));

    wm.add_collision_shape(leg);
    wm.add_collision_shape(top);

    btTransform leg_trans, top_trans;
    leg_trans.setIdentity();
    top_trans.setIdentity();

    leg_trans.setOrigin(btVector3(0, 0, -(total_fork_width / 2.0 - fork_width / 2.0)));
    top_trans.setOrigin(btVector3(0, m_fork_length / 2.0, 0));

    m_fork_shape->addChildShape(leg_trans, leg);
    m_fork_shape->addChildShape(top_trans, top);
  }

  wm.add_collision_shape(m_fork_shape);

  // Make the unicycle wheel
  m_wheel_shape = new btCylinderShapeZ(btVector3(m_wheel_radius, m_wheel_radius, wheel_width / 2.0));
  wm.add_collision_shape(m_wheel_shape);

  // Make the drive wheel
  m_drive_shape = new btCylinderShape(btVector3(m_drive_radius, m_drive_radius / 10.0, m_drive_radius));
  wm.add_collision_shape(m_drive_shape);
}

void Unicycle::create_rigid_bodies(WorldManager& wm, btTransform const& trans)
{
  btTransform wheel_trans = trans;
  wheel_trans.getOrigin() += btVector3(0, m_wheel_radius, 0);

  btTransform fork_trans = wheel_trans;
  fork_trans.getOrigin() += btVector3(0, m_fork_length, 0);

  btTransform drive_trans = fork_trans;
  drive_trans.getOrigin() += btVector3(0, m_drive_radius / 4.0, 0);

  // This creates a fork with center of mass exactly m_fork_length above trans.
  {
    btTransform principal_trans; //< Transform from principle inertial axes to global coords.
    btTransform local_trans;     //< Transform from body coords to global coords.
    btVector3 principal_inertia; //< Principle moments of inertia.
    btVector3 local_inertia;     //< Local moments of inertia.

    btScalar masses[2] = {0, m_rider_mass};

    m_fork_shape->calculatePrincipalAxisTransform(masses, principal_trans, principal_inertia);

    m_fork_shape->updateChildTransform(0, principal_trans.inverse() * m_fork_shape->getChildTransform(0));
    m_fork_shape->updateChildTransform(1, principal_trans.inverse() * m_fork_shape->getChildTransform(1));

    m_fork_shape->calculateLocalInertia(m_rider_mass, local_inertia);

    btRigidBody::btRigidBodyConstructionInfo fork_CI(m_rider_mass, 0, m_fork_shape, local_inertia);
    fork_CI.m_startWorldTransform = fork_trans;
    m_fork_body = new btRigidBody(fork_CI);
    wm.dynamics_world()->addRigidBody(m_fork_body);
  }

  // Create the wheel.
  {
    m_wheel_body = wm.add_rigid_body(m_wheel_mass, wheel_trans, m_wheel_shape);
    m_wheel_body->setFriction(1000.0);
  }

  // Create the drive wheel.
  {
    m_drive_body = wm.add_rigid_body(m_drive_mass, drive_trans, m_drive_shape);
    m_drive_body->setDamping(0.0, 0.2);
  }
}

void Unicycle::add_to_manager(WorldManager& wm) throw(UnicycleAlreadyInitializedError)
{
  if (m_fork_shape || m_wheel_shape) {
    throw UnicycleAlreadyInitializedError();
  }

  create_collision_shapes(wm);
  create_rigid_bodies(wm, reset_transform);

  // Add axle constraint
  {
    btVector3 pivot_in_wheel(0, 0, 0);
    btVector3 pivot_in_fork(0, -m_fork_length, 0);
    btVector3 axis_in_wheel(0, 0, 1);
    btVector3 axis_in_fork(0, 0, 1);

    btTypedConstraint* axle = new btHingeConstraint(*m_wheel_body, *m_fork_body,
                                                    pivot_in_wheel, pivot_in_fork,
                                                    axis_in_wheel, axis_in_fork);

    wm.dynamics_world()->addConstraint(axle);
  }

  // Add drive axle constraint
  {
    btVector3 pivot_in_drive(0, -m_drive_radius / 20.0, 0);
    btVector3 pivot_in_fork(0, m_fork_length / 20.0, 0);
    btVector3 axis_in_drive(0, 1, 0);
    btVector3 axis_in_fork(0, 1, 0);

    btTypedConstraint* axle = new btHingeConstraint(*m_drive_body, *m_fork_body,
                                                    pivot_in_drive, pivot_in_fork,
                                                    axis_in_drive, axis_in_fork);

    if (m_drive_limits) {
      static_cast<btHingeConstraint*>(axle)->setLimit(-M_PI + 0.2, M_PI - 0.2);
    }

    wm.dynamics_world()->addConstraint(axle);
  }

  m_fork_body->setActivationState(DISABLE_DEACTIVATION);
  m_wheel_body->setActivationState(DISABLE_DEACTIVATION);
  m_drive_body->setActivationState(DISABLE_DEACTIVATION);
}

void Unicycle::apply_wheel_impulse(btScalar imp)
{
  btVector3 impulse_in_wheel(0, 0, imp);
  btVector3 impulse_in_world = m_wheel_body->getWorldTransform().getBasis() * impulse_in_wheel;

  m_wheel_body->applyTorqueImpulse(impulse_in_world);
  m_fork_body->applyTorqueImpulse(-impulse_in_world);
}

void Unicycle::apply_drive_impulse(btScalar imp)
{
  btVector3 impulse_in_drive(0, imp, 0);
  btVector3 impulse_in_world = m_drive_body->getWorldTransform().getBasis() * impulse_in_drive;

  m_drive_body->applyTorqueImpulse(impulse_in_world);
  m_fork_body->applyTorqueImpulse(-impulse_in_world);
}

void Unicycle::reset(double random, btTransform const& trans)
{
  btVector3 moving(0, 0, 0);
  btVector3 zeros(0, 0, 0);

  reset_position(random, trans);

  m_wheel_body->setLinearVelocity(moving);
  m_wheel_body->setAngularVelocity(zeros);

  m_fork_body->setLinearVelocity(moving);
  m_fork_body->setAngularVelocity(zeros);

  m_drive_body->setLinearVelocity(moving);
  m_drive_body->setAngularVelocity(zeros);
}

void Unicycle::reset_position(double random, btTransform const& trans)
{
  btTransform wheel_trans = trans;
  btTransform fork_trans = trans;
  btTransform drive_trans = trans;

  if (random > 0.0) {
    btVector3 random_vec(util::rand(-1,1), util::rand(-1,1), util::rand(-1,1));
    btScalar random_ang(util::rand(0, random));

    btQuaternion rot(random_vec, random_ang);

    wheel_trans.setRotation(rot);
    fork_trans.setRotation(rot);
    drive_trans.setRotation(rot);
  }

  btVector3 wheel_origin(0, m_wheel_radius, 0);
  btVector3 fork_origin = wheel_origin + wheel_trans.getBasis() * btVector3(0, m_fork_length, 0);
  btVector3 drive_origin = fork_origin + wheel_trans.getBasis() * btVector3(0, m_drive_radius / 4.0, 0);

  wheel_trans.getOrigin() += wheel_origin;
  fork_trans.getOrigin() += fork_origin;
  drive_trans.getOrigin() += drive_origin;

  m_wheel_body->setWorldTransform(wheel_trans);
  m_fork_body->setWorldTransform(fork_trans);
  m_drive_body->setWorldTransform(drive_trans);
}

// Calculate position of lowest vertex in wheel
btVector3 Unicycle::contact_point() const
{
  btTransform wheel_trans = m_wheel_body->getWorldTransform();

  btVector3 world_down(0, -1, 0);
  btVector3 support_axis_in_wheel = world_down * wheel_trans.getBasis();
  btVector3 contact_point_in_wheel = m_wheel_shape->localGetSupportVertexWithoutMarginNonVirtual(support_axis_in_wheel);
  btVector3 contact_point = wheel_trans * contact_point_in_wheel;
  return contact_point;
}

void Unicycle::compute_state(btScalar timestep)
{
  btTransform wheel_trans = m_wheel_body->getWorldTransform();
  btTransform fork_trans = m_fork_body->getWorldTransform();

  btVector3 c_point = contact_point();

  // Roll: angle between <world y-vector>, and <vector from contact point to center of wheel>.
  btVector3 bottom_spoke;
  {
    btVector3 world_up(0, 1, 0);
    bottom_spoke = wheel_trans.getOrigin() - c_point;

    m_roll = world_up.angle(bottom_spoke);
  }

  // Yaw: angle between <world x-vector>, and <vector from center of wheel to point on wheel edge at 90deg to contact point>
  {
    // Initial wheel axis
    btVector3 axle(0, 0, 1);
    btTransform rotation_in_wheel(btQuaternion(axle, M_PI / 2.0));

    btVector3 forward_point_in_wheel = rotation_in_wheel * (wheel_trans.inverse() * c_point);
    btVector3 forward_point = wheel_trans * forward_point_in_wheel;

    btVector3 forward_spoke = wheel_trans.getOrigin() - forward_point;

    // This should define zero to be heading directly to the right on screen
    m_yaw = std::atan2(forward_spoke.getZ(), -forward_spoke.getX());
  }

  // Pitch: angle between <vector from center of wheel to center of mass of seat post> and <vector from contact point to center of wheel>
  {
    btVector3 bottom_spoke_in_fork = bottom_spoke * fork_trans.getBasis();
    m_pitch = std::atan2(bottom_spoke_in_fork.getX(), bottom_spoke_in_fork.getY());
  }

  // Wheel angular velocity about wheel axle
  // Yaw angular velocity about world Y axis
  {
    btVector3 wheel_vel = m_wheel_body->getAngularVelocity();
    btVector3 wheel_vel_in_wheel = wheel_vel * wheel_trans.getBasis();
    m_wheel_velocity = wheel_vel_in_wheel.getZ();
    m_yaw_velocity = wheel_vel.getY();
  }

  // Pitch/roll velocity
  {
    m_pitch_velocity = (m_pitch - m_last_pitch) / timestep;
    m_roll_velocity = (m_roll - m_last_roll) / timestep;
  }

  m_last_pitch = m_pitch;
  m_last_roll = m_roll;

  apply_friction(timestep);
}

void Unicycle::apply_friction(btScalar timestep)
{
  btScalar torque = - fric_kinetic * m_yaw_velocity - fric_static * ((m_yaw_velocity > 0) - (m_yaw_velocity < 0));

  btVector3 impulse_in_world(0, timestep * torque, 0);

  m_wheel_body->applyTorqueImpulse(impulse_in_world);
}

btScalar Unicycle::kinetic_energy() const
{
  btScalar lke = 0;
  lke += 0.5 * m_rider_mass * m_fork_body->getLinearVelocity().length2();
  lke += 0.5 * m_drive_mass * m_drive_body->getLinearVelocity().length2();
  lke += 0.5 * m_wheel_mass * m_wheel_body->getLinearVelocity().length2();

  btScalar rke = 0;
  btVector3 w_fork = m_fork_body->getAngularVelocity();
  btVector3 w_drive = m_drive_body->getAngularVelocity();
  btVector3 w_wheel = m_wheel_body->getAngularVelocity();
  rke += 0.5 * w_fork.dot(m_fork_body->getInvInertiaTensorWorld().inverse() * w_fork);
  rke += 0.5 * w_drive.dot(m_drive_body->getInvInertiaTensorWorld().inverse() * w_drive);
  rke += 0.5 * w_wheel.dot(m_wheel_body->getInvInertiaTensorWorld().inverse() * w_wheel);

  return lke + rke;
}

btScalar Unicycle::potential_energy() const
{
  btScalar mh = 0;
  btScalar g = 10;
  mh += m_rider_mass * m_fork_body->getWorldTransform().getOrigin().getY();
  mh += m_drive_mass * m_drive_body->getWorldTransform().getOrigin().getY();
  mh += m_wheel_mass * m_wheel_body->getWorldTransform().getOrigin().getY();
  return mh * g;
}

btScalar Unicycle::energy() const
{
  return potential_energy() + kinetic_energy();
}

btVector3 const& Unicycle::origin() const
{
  return m_wheel_body->getWorldTransform().getOrigin();
}

btScalar Unicycle::yaw() const { return m_yaw; }
btScalar Unicycle::pitch() const { return m_pitch; }
btScalar Unicycle::roll() const { return m_roll; }

btScalar Unicycle::wheel_velocity() const { return m_wheel_velocity; }
btScalar Unicycle::yaw_velocity() const { return m_yaw_velocity; }
btScalar Unicycle::pitch_velocity() const { return m_pitch_velocity; }
btScalar Unicycle::roll_velocity() const { return m_roll_velocity; }

