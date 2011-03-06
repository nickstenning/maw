#include <stdexcept>
#include <iostream>
#include <cmath>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"

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

    wm.addCollisionShape(fork);
    wm.addCollisionShape(seat);

    btTransform forkTrans, seatTrans;
    forkTrans.setIdentity();
    seatTrans.setIdentity();

    forkTrans.setOrigin(btVector3(0, 0, -(totalForkWidth / 2.0 - m_forkWidth / 2.0)));
    seatTrans.setOrigin(btVector3(0, m_forkLength / 2.0, 0));

    m_forkShape->addChildShape(forkTrans, fork);
    m_forkShape->addChildShape(seatTrans, seat);
  }

  wm.addCollisionShape(m_forkShape);
}

// This create a fork/seat with center of mass exactly m_forkLength above trans.
void Unicycle::createForkBody(WorldManager& wm, btTransform const& trans)
{
  btTransform principalTrans; //< Transform from principle inertial axes to global coords.
  btTransform localTrans;     //< Transform from body coords to global coords.
  btVector3 principalInertia; //< Principle moments of inertia.
  btVector3 localInertia;     //< Local moments of inertia.

  btScalar masses[2] = {0, m_forkMass};

  m_forkShape->calculatePrincipalAxisTransform(masses, principalTrans, principalInertia);

  m_forkShape->updateChildTransform(0, principalTrans.inverse() * m_forkShape->getChildTransform(0));
  m_forkShape->updateChildTransform(1, principalTrans.inverse() * m_forkShape->getChildTransform(1));

  m_forkShape->calculateLocalInertia(m_forkMass, localInertia);

  btRigidBody::btRigidBodyConstructionInfo forkCI(m_forkMass, 0,  m_forkShape, localInertia);
  forkCI.m_startWorldTransform = trans;
  m_forkBody = new btRigidBody(forkCI);

  wm.dynamicsWorld()->addRigidBody(m_forkBody);
}

void Unicycle::createWheelShape(WorldManager& wm)
{
  m_wheelShape = new btCylinderShapeZ(btVector3(m_wheelRadius, m_wheelRadius, m_wheelWidth/2));
  wm.addCollisionShape(m_wheelShape);
}

// Transform defines position of center of wheel.
void Unicycle::createWheelBody(WorldManager& wm, btTransform const& trans)
{
  m_wheelBody = wm.addRigidBody(2.5, trans, m_wheelShape);
  m_wheelBody->setFriction(20.0);
}

void Unicycle::addToManager(WorldManager& wm)
{
  if (m_forkShape || m_wheelShape) {
    throw std::runtime_error("Can't add a Unicycle to a WorldManager more than once.");
  }

  // createForkShape(wm);
  createWheelShape(wm);

  btTransform wheelTrans = resetTransform;
  wheelTrans.getOrigin() += btVector3(0, m_wheelRadius, 0);

  // btTransform forkTrans = wheelTrans;
  // forkTrans.getOrigin() += btVector3(0, m_forkLength, 0);

  // createForkBody(wm, forkTrans);
  createWheelBody(wm, wheelTrans);

  // Add axle constraint
  if (0) {
    btVector3 pivotInWheel(0, 0, m_wheelWidth);
    btVector3 pivotInFork(0, -m_forkLength, m_wheelWidth);
    btVector3 axisInWheel(0, 0, 1);
    btVector3 axisInFork(0, 0, 1);

    btTypedConstraint* axle = new btHingeConstraint(*m_wheelBody, *m_forkBody, pivotInWheel, pivotInFork, axisInWheel, axisInFork);

    wm.dynamicsWorld()->addConstraint(axle);
  }

  // m_forkBody->setActivationState(DISABLE_DEACTIVATION);
  m_wheelBody->setActivationState(DISABLE_DEACTIVATION);

}

void Unicycle::applyWheelImpulse(double impulse)
{
  if (!m_wheelBody) { return; }

  btVector3 impulseLocal(0, 0, impulse);
  btVector3 impulseWorld = m_wheelBody->getWorldTransform().getBasis() * impulseLocal;

  m_wheelBody->applyTorqueImpulse(impulseWorld);
}

void Unicycle::applyForkImpulse(double impulse)
{
  if (!m_forkBody) { return; }

  btVector3 impulseLocal(0, impulse, 0);
  btVector3 impulseWorld = m_forkBody->getWorldTransform().getBasis() * impulseLocal;

  m_forkBody->applyTorqueImpulse(impulseWorld);
}

void Unicycle::reset(btTransform const& trans)
{
  btVector3 zeros(0, 0, 0);

  transform(trans);

  if (m_wheelBody) {
    m_wheelBody->setLinearVelocity(zeros);
    m_wheelBody->setAngularVelocity(zeros);
  }

  if (m_forkBody) {
    m_forkBody->setLinearVelocity(zeros);
    m_forkBody->setAngularVelocity(zeros);
  }
}

void Unicycle::translate(btScalar x, btScalar y, btScalar z)
{
  btTransform t = transform();
  t.getOrigin() += btVector3(x, y, z);
  transform(t);
}

btTransform Unicycle::transform()
{
  if (m_wheelBody) {
    m_transform = m_wheelBody->getWorldTransform();
  }
  btTransform t = m_transform;
  t.getOrigin() -= btVector3(0, m_wheelRadius, 0);

  return t;
}

Unicycle& Unicycle::transform(btTransform const& t)
{
  m_transform = t;

  if (m_wheelBody) {
    btTransform wheelTrans = m_transform;
    wheelTrans.getOrigin() += btVector3(0, m_wheelRadius, 0);
    m_wheelBody->setWorldTransform(wheelTrans);
  }

  if (m_forkBody) {
    btTransform forkTrans = m_transform;
    forkTrans.getOrigin() += btVector3(0, m_wheelRadius + m_forkLength, 0);
    m_forkBody->setWorldTransform(forkTrans);
  }

  return *this;
}


void Unicycle::resetAxis()
{
  // Initial wheel axis
  btVector3 axle(0, 0, 1);

  btTransform wheelTrans = m_wheelBody->getWorldTransform();

  std::cout << "[" << axle.getX() << ", " << axle.getY() << ", " << axle.getZ() <<  "]" << "\n";

  btTransform wheelRotation(btQuaternion(axle, 3.14));

  m_wheelBody->setWorldTransform(wheelTrans * wheelRotation);
}

void Unicycle::updateAngles()
{
  btTransform wheelTrans = m_wheelBody->getWorldTransform();
  btVector3   wheelOrigin = wheelTrans.getOrigin();
  btMatrix3x3 wheelRot = wheelTrans.getBasis();

  std::cout << "x=" << wheelOrigin.getX() << "\tz=" << wheelOrigin.getZ() << "\ty=" << wheelOrigin.getY() << "\n";
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

Unicycle& Unicycle::yaw(btScalar)
{
  return *this;
}

Unicycle& Unicycle::pitch(btScalar)
{
  return *this;
}

Unicycle& Unicycle::roll(btScalar)
{
  return *this;
}

