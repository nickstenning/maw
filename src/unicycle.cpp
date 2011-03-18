#include <stdexcept>
#include <iostream>
#include <cmath>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"
#include "util.h"

const btTransform Unicycle::resetTransform = btTransform(btQuaternion::getIdentity());

Unicycle::Unicycle()
  : m_forkWidth   ( 0.2  )
  , m_forkLength  ( 4.0  )
  , m_wheelWidth  ( 0.25 )
  , m_wheelRadius ( 2.0  )
  , m_forkMass    ( 10.0 )
  , m_wheelMass   ( 2.5  )
  , m_yaw(0)
  , m_pitch(0)
  , m_roll(0)
  , m_wheel_velocity(0)
  , m_seat_velocity(0)
  , m_yaw_velocity(0)
  , m_yawRestore(0)
  , m_transform(Unicycle::resetTransform)
  , m_forkShape(0)
  , m_wheelShape(0)
  , m_forkBody(0)
  , m_wheelBody(0)
{}

void Unicycle::createForkShape(WorldManager& wm)
{
  // Make the unicycle fork
  m_forkShape = new btCompoundShape();
  {
    float totalForkWidth = 2.0 * (m_forkWidth + m_wheelWidth);
    btCollisionShape* fork = new btBoxShape(btVector3(m_forkWidth / 2.0, m_forkLength / 2.0, m_forkWidth / 2.0));
    btCollisionShape* seat = new btCapsuleShapeX(totalForkWidth / 2.0, m_forkLength / 5.0);

    wm.add_collision_shape(fork);
    wm.add_collision_shape(seat);

    btTransform forkTrans, seatTrans;
    forkTrans.setIdentity();
    seatTrans.setIdentity();

    forkTrans.setOrigin(btVector3(0, 0, -(totalForkWidth / 2.0 - m_forkWidth / 2.0)));
    seatTrans.setOrigin(btVector3(0, m_forkLength / 2.0, 0));

    m_forkShape->addChildShape(forkTrans, fork);
    m_forkShape->addChildShape(seatTrans, seat);
  }

  wm.add_collision_shape(m_forkShape);
}

// This create a fork/seat with center of mass exactly m_forkLength above trans.
void Unicycle::createForkBody(WorldManager& wm, btTransform const& trans)
{
  btTransform principalTrans; //< Transform from principle inertial axes to global coords.
  btTransform localTrans;     //< Transform from body coords to global coords.
  btVector3 principalInertia; //< Principle moments of inertia.
  btVector3 local_inertia;     //< Local moments of inertia.

  btScalar masses[2] = {0, m_forkMass};

  m_forkShape->calculatePrincipalAxisTransform(masses, principalTrans, principalInertia);

  m_forkShape->updateChildTransform(0, principalTrans.inverse() * m_forkShape->getChildTransform(0));
  m_forkShape->updateChildTransform(1, principalTrans.inverse() * m_forkShape->getChildTransform(1));

  m_forkShape->calculateLocalInertia(m_forkMass, local_inertia);

  btRigidBody::btRigidBodyConstructionInfo forkCI(m_forkMass, 0,  m_forkShape, local_inertia);
  forkCI.m_startWorldTransform = trans;
  m_forkBody = new btRigidBody(forkCI);

  wm.dynamics_world()->addRigidBody(m_forkBody);
}

void Unicycle::createWheelShape(WorldManager& wm)
{
  m_wheelShape = new btCylinderShapeZ(btVector3(m_wheelRadius, m_wheelRadius, m_wheelWidth/2));
  wm.add_collision_shape(m_wheelShape);
}

// Transform defines position of center of wheel.
void Unicycle::createWheelBody(WorldManager& wm, btTransform const& trans)
{
  m_wheelBody = wm.add_rigid_body(2.5, trans, m_wheelShape);
  m_wheelBody->setFriction(20.0);
}

void Unicycle::add_to_manager(WorldManager& wm)
{
  if (m_forkShape || m_wheelShape) {
    throw std::runtime_error("Can't add a Unicycle to a WorldManager more than once.");
  }

  createForkShape(wm);
  createWheelShape(wm);

  btTransform wheelTrans = resetTransform;
  wheelTrans.getOrigin() += btVector3(0, m_wheelRadius, 0);

  btTransform forkTrans = wheelTrans;
  forkTrans.getOrigin() += btVector3(0, m_forkLength, 0);

  createForkBody(wm, forkTrans);
  createWheelBody(wm, wheelTrans);

  // Add axle constraint
  {
    btVector3 pivotInWheel(0, 0, m_wheelWidth);
    btVector3 pivotInFork(0, -m_forkLength, m_wheelWidth);
    btVector3 axisInWheel(0, 0, 1);
    btVector3 axisInFork(0, 0, 1);

    btTypedConstraint* axle = new btHingeConstraint(*m_wheelBody, *m_forkBody, pivotInWheel, pivotInFork, axisInWheel, axisInFork);

    wm.dynamics_world()->addConstraint(axle);
  }

  m_forkBody->setActivationState(DISABLE_DEACTIVATION);
  m_wheelBody->setActivationState(DISABLE_DEACTIVATION);

}

void Unicycle::apply_wheel_impulse(double impulse, bool)
{
  btVector3 impulseLocal(0, 0, impulse * PITCH_BANG_SIZE);
  btVector3 impulseWorld = m_wheelBody->getWorldTransform().getBasis() * impulseLocal;

  m_wheelBody->applyTorqueImpulse(impulseWorld);
}

void Unicycle::apply_fork_impulse(double impulse, bool)
{
  // if (!force && m_yawRestore > 0) {
  //   m_yawRestore -= 1;
  //   return;
  // }

  btVector3 impulseLocal(0, impulse * YAW_BANG_SIZE, 0);
  btVector3 impulseWorld = m_forkBody->getWorldTransform().getBasis() * impulseLocal;

  m_forkBody->applyTorqueImpulse(impulseWorld);

  // if (!force) { m_yawRestore = 10; }
}

void Unicycle::reset(btTransform const& trans)
{
  m_yawRestore = 0;

  btVector3 zeros(0, 0, 0);

  transform(trans);

  m_wheelBody->setLinearVelocity(zeros);
  m_wheelBody->setAngularVelocity(zeros);

  m_forkBody->setLinearVelocity(zeros);
  m_forkBody->setAngularVelocity(zeros);
}

void Unicycle::translate(btScalar x, btScalar y, btScalar z)
{
  btTransform t = transform();
  t.getOrigin() += btVector3(x, y, z);
  transform(t);
}

btTransform Unicycle::transform()
{
  m_transform = m_wheelBody->getWorldTransform();
  btTransform t = m_transform;
  t.getOrigin() -= btVector3(0, m_wheelRadius, 0);

  return t;
}

Unicycle& Unicycle::transform(btTransform const& t)
{
  m_transform = t;

  btTransform wheelTrans = m_transform;
  btTransform forkTrans = m_transform;


  btVector3 randomVec(util::rand(-1,1), util::rand(-1,1), util::rand(-1,1));
  btScalar randomAng(util::rand(0, 0.1));

  wheelTrans.getOrigin() += btVector3(0, m_wheelRadius, 0).rotate(randomVec, randomAng);
  forkTrans.getOrigin() += btVector3(0, m_wheelRadius + m_forkLength, 0).rotate(randomVec, randomAng);

  wheelTrans.setRotation(wheelTrans.getRotation() * btQuaternion(randomVec, randomAng));
  forkTrans.setRotation(forkTrans.getRotation() * btQuaternion(randomVec, randomAng));

  m_wheelBody->setWorldTransform(wheelTrans);
  m_forkBody->setWorldTransform(forkTrans);

  return *this;
}

void Unicycle::compute_state()
{
  btTransform wheelTrans = m_wheelBody->getWorldTransform();
  btVector3   wheelOrigin = wheelTrans.getOrigin();
  btTransform forkTrans = m_forkBody->getWorldTransform();
  btVector3   forkOrigin = forkTrans.getOrigin();

  // Calculate position of lowest vertex in wheel
  btVector3 contactPoint;
  btVector3 contactPointInWheel;
  {
    btVector3 worldDown(0, -1, 0);
    btVector3 supportAxisInWheel = worldDown * wheelTrans.getBasis();
    contactPointInWheel = m_wheelShape->localGetSupportVertexWithoutMarginNonVirtual(supportAxisInWheel);
    contactPoint = wheelTrans * contactPointInWheel;
  }

  // Roll: angle between <world y-vector>, and <vector from contact point to center of wheel>.
  btVector3 bottomSpoke;
  {
    btVector3 worldUp(0, 1, 0);
    bottomSpoke = wheelOrigin - contactPoint;

    m_roll = worldUp.angle(bottomSpoke);
  }

  // Yaw: angle between <world x-vector>, and <vector from center of wheel to point on wheel edge at 90deg to contact point>
  {
    // Initial wheel axis
    btVector3 axle(0, 0, 1);
    btTransform rotationInWheel(btQuaternion(axle, M_PI / 2.0));

    btVector3 forwardPointInWheel = rotationInWheel * contactPointInWheel;
    btVector3 forwardPoint = wheelTrans * forwardPointInWheel;

    btVector3 forwardSpoke = wheelOrigin - forwardPoint;

    // This should define zero to be heading directly to the right on screen
    m_yaw = std::atan2(forwardSpoke.getZ(), -forwardSpoke.getX());
  }

  // Pitch: angle between <vector from center of wheel to center of mass of seat post> and <vector from contact point to center of wheel>
  {
    btVector3 bottomSpokeInFork = bottomSpoke * forkTrans.getBasis();
    m_pitch = std::atan2(bottomSpokeInFork.getX(), bottomSpokeInFork.getY());
  }

  // Wheel angular velocity about wheel axis
  {
    btVector3 wheelVel = m_wheelBody->getAngularVelocity();
    btVector3 wheelVelInWheel = wheelVel * wheelTrans.getBasis();
    m_wheel_velocity = wheelVelInWheel.getZ();
  }

  // Seat angular velocity about wheel axis
  {
    // btVector3 seatVel = m_forkBody->getAngularVelocity();
    // btVector3 seatVelInWheel = seatVel * forkTrans.getBasis();
    // m_seat_velocity = seatVelInWheel.getX();
    // std::cout << m_seat_velocity << "\n";
  }

  // Yaw velocity about wheel axis
  {
    btVector3 forkVel = m_forkBody->getAngularVelocity();
    btVector3 forkVelInFork = forkVel * forkTrans.getBasis();
    m_yaw_velocity = forkVelInFork.getZ();
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

btScalar Unicycle::seat_velocity() const
{
  return m_seat_velocity;
}

btScalar Unicycle::yaw_velocity() const
{
  return m_yaw_velocity;
}

