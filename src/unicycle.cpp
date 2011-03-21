#include <stdexcept>
#include <iostream>
#include <cmath>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"
#include "util.h"

const btTransform Unicycle::reset_transform = btTransform(btQuaternion::getIdentity());

Unicycle::Unicycle(
  btScalar fork_width,
  btScalar fork_length,
  btScalar wheel_width,
  btScalar wheel_radius,
  btScalar seat_mass,
  btScalar wheel_mass
) : m_fork_width(fork_width)
  , m_fork_length(fork_length)
  , m_wheel_width(wheel_width)
  , m_wheel_radius(wheel_radius)
  , m_seat_mass(seat_mass)
  , m_wheel_mass(wheel_mass)
  , m_yaw(0)
  , m_pitch(0)
  , m_roll(0)
  , m_wheel_velocity(0)
  , m_yaw_velocity(0)
  , m_transform(Unicycle::reset_transform)
  , m_fork_shape(NULL)
  , m_wheel_shape(NULL)
  , m_fork_body(NULL)
  , m_wheel_body(NULL)
{}

void Unicycle::create_collision_shapes(WorldManager& wm)
{
  // Make the unicycle fork
  m_fork_shape = new btCompoundShape();
  {
    float total_fork_width = 2.0 * (m_fork_width + m_wheel_width);
    btCollisionShape* fork = new btBoxShape(btVector3(m_fork_width / 2.0, m_fork_length / 2.0, m_fork_width / 2.0));
    btCollisionShape* seat = new btCapsuleShapeX(total_fork_width / 2.0, m_fork_length / 5.0);

    wm.add_collision_shape(fork);
    wm.add_collision_shape(seat);

    btTransform fork_trans, seat_trans;
    fork_trans.setIdentity();
    seat_trans.setIdentity();

    fork_trans.setOrigin(btVector3(0, 0, -(total_fork_width / 2.0 - m_fork_width / 2.0)));
    seat_trans.setOrigin(btVector3(0, m_fork_length / 2.0, 0));

    m_fork_shape->addChildShape(fork_trans, fork);
    m_fork_shape->addChildShape(seat_trans, seat);
  }

  wm.add_collision_shape(m_fork_shape);

  // Make the unicycle wheel
  m_wheel_shape = new btCylinderShapeZ(btVector3(m_wheel_radius, m_wheel_radius, m_wheel_width/2));
  wm.add_collision_shape(m_wheel_shape);
}

void Unicycle::create_rigid_bodies(WorldManager& wm, btTransform const& trans)
{
  btTransform wheel_trans = trans;
  wheel_trans.getOrigin() += btVector3(0, m_wheel_radius, 0);

  btTransform fork_trans = wheel_trans;
  fork_trans.getOrigin() += btVector3(0, m_fork_length, 0);

  // This creates a fork/seat with center of mass exactly m_forkLength above trans.
  {
    btTransform principal_trans; //< Transform from principle inertial axes to global coords.
    btTransform local_trans;     //< Transform from body coords to global coords.
    btVector3 principal_inertia; //< Principle moments of inertia.
    btVector3 local_inertia;     //< Local moments of inertia.

    btScalar masses[2] = {0, m_seat_mass};

    m_fork_shape->calculatePrincipalAxisTransform(masses, principal_trans, principal_inertia);

    m_fork_shape->updateChildTransform(0, principal_trans.inverse() * m_fork_shape->getChildTransform(0));
    m_fork_shape->updateChildTransform(1, principal_trans.inverse() * m_fork_shape->getChildTransform(1));

    m_fork_shape->calculateLocalInertia(m_seat_mass, local_inertia);

    btRigidBody::btRigidBodyConstructionInfo fork_CI(m_seat_mass, 0, m_fork_shape, local_inertia);
    fork_CI.m_startWorldTransform = fork_trans;
    m_fork_body = new btRigidBody(fork_CI);
    wm.dynamics_world()->addRigidBody(m_fork_body);
  }

  // Create the wheel.
  {
    m_wheel_body = wm.add_rigid_body(m_wheel_mass, wheel_trans, m_wheel_shape);
    m_wheel_body->setFriction(20.0);
  }
}

void Unicycle::add_to_manager(WorldManager& wm)
{
  if (m_fork_shape || m_wheel_shape) {
    throw std::runtime_error("Can't add a Unicycle to a WorldManager more than once.");
  }

  create_collision_shapes(wm);
  create_rigid_bodies(wm, reset_transform);

  // Add axle constraint
  {
    btVector3 pivot_in_wheel(0, 0, m_wheel_width);
    btVector3 pivot_in_fork(0, -m_fork_length, m_wheel_width);
    btVector3 axis_in_wheel(0, 0, 1);
    btVector3 axis_in_fork(0, 0, 1);

    btTypedConstraint* axle = new btHingeConstraint(*m_wheel_body, *m_fork_body,
                                                    pivot_in_wheel, pivot_in_fork,
                                                    axis_in_wheel, axis_in_fork);

    wm.dynamics_world()->addConstraint(axle);
  }

  m_fork_body->setActivationState(DISABLE_DEACTIVATION);
  m_wheel_body->setActivationState(DISABLE_DEACTIVATION);
}

void Unicycle::apply_wheel_impulse(btScalar impulse)
{
  btVector3 impulse_in_wheel(0, 0, impulse);
  btVector3 impulse_in_world = m_wheel_body->getWorldTransform().getBasis() * impulse_in_wheel;

  m_wheel_body->applyTorqueImpulse(impulse_in_world);
}

void Unicycle::apply_fork_impulse(btScalar impulse)
{
  btVector3 impulse_in_fork(0, impulse, 0);
  btVector3 impulse_in_world = m_fork_body->getWorldTransform().getBasis() * impulse_in_fork;

  m_fork_body->applyTorqueImpulse(impulse_in_world);
}

void Unicycle::reset(btTransform const& trans)
{
  btVector3 zeros(0, 0, 0);

  transform(trans);

  m_wheel_body->setLinearVelocity(zeros);
  m_wheel_body->setAngularVelocity(zeros);

  m_fork_body->setLinearVelocity(zeros);
  m_fork_body->setAngularVelocity(zeros);
}

void Unicycle::translate(btScalar x, btScalar y, btScalar z)
{
  btTransform t = transform();
  t.getOrigin() += btVector3(x, y, z);
  transform(t);
}

btTransform Unicycle::transform()
{
  m_transform = m_wheel_body->getWorldTransform();
  btTransform t = m_transform;
  btVector3 bottom_spoke = t.getOrigin() - contact_point();
  t.getOrigin() -= bottom_spoke;

  return t;
}

Unicycle& Unicycle::transform(btTransform const& t)
{
  m_transform = t;

  btTransform wheel_trans = m_transform;
  btTransform fork_trans = m_transform;

  btVector3 random_vec(util::rand(-1,1), util::rand(-1,1), util::rand(-1,1));
  btScalar random_ang(util::rand(0, 0.1));

  wheel_trans.getOrigin() += btVector3(0, m_wheel_radius, 0).rotate(random_vec, random_ang);
  fork_trans.getOrigin() += btVector3(0, m_wheel_radius + m_fork_length, 0).rotate(random_vec, random_ang);

  wheel_trans.setRotation(wheel_trans.getRotation() * btQuaternion(random_vec, random_ang));
  fork_trans.setRotation(fork_trans.getRotation() * btQuaternion(random_vec, random_ang));

  m_wheel_body->setWorldTransform(wheel_trans);
  m_fork_body->setWorldTransform(fork_trans);

  return *this;
}

// Calculate position of lowest vertex in wheel
btVector3 Unicycle::contact_point()
{
  btTransform wheel_trans = m_wheel_body->getWorldTransform();

  btVector3 world_down(0, -1, 0);
  btVector3 support_axis_in_wheel = world_down * wheel_trans.getBasis();
  btVector3 contact_point_in_wheel = m_wheel_shape->localGetSupportVertexWithoutMarginNonVirtual(support_axis_in_wheel);
  btVector3 contact_point = wheel_trans * contact_point_in_wheel;
  return contact_point;
}

void Unicycle::compute_state()
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

  // Wheel angular velocity about wheel axis
  {
    btVector3 wheel_vel = m_wheel_body->getAngularVelocity();
    btVector3 wheel_vel_in_wheel = wheel_vel * wheel_trans.getBasis();
    m_wheel_velocity = wheel_vel_in_wheel.getZ();
  }

  // Yaw velocity about wheel axis
  {
    btVector3 fork_vel = m_fork_body->getAngularVelocity();
    btVector3 fork_vel_in_fork = fork_vel * fork_trans.getBasis();
    m_yaw_velocity = fork_vel_in_fork.getZ();
  }
}

btScalar Unicycle::yaw() const
{
  return m_yaw;
}

btScalar Unicycle::pitch() const
{
  return m_pitch;
}

btScalar Unicycle::roll() const
{
  return m_roll;
}

btScalar Unicycle::wheel_velocity() const
{
  return m_wheel_velocity;
}

btScalar Unicycle::yaw_velocity() const
{
  return m_yaw_velocity;
}

