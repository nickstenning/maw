#include <stdexcept>
#include <cmath>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"

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

  localTrans = trans;
  localTrans.getOrigin() += btVector3(0, m_forkLength, 0);

  m_forkShape->calculateLocalInertia(m_forkMass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(localTrans);
  m_forkBody = new btRigidBody(m_forkMass, motionState, m_forkShape, localInertia);

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

void Unicycle::addToManager(WorldManager& wm, btTransform const& trans)
{
  if (m_forkShape || m_wheelShape) {
    throw std::runtime_error("Can't add a Unicycle to a WorldManager more than once.");
  }

  createForkShape(wm);
  createWheelShape(wm);

  createForkBody(wm, trans);
  createWheelBody(wm, trans);

  // Add axle constraint
  {
    btVector3 pivotInWheel(0, 0, m_wheelWidth);
    btVector3 pivotInFork(0, -m_forkLength, m_wheelWidth);
    btVector3 axisInWheel(0, 0, 1);
    btVector3 axisInFork(0, 0, 1);

    btTypedConstraint* axle = new btHingeConstraint(*m_wheelBody, *m_forkBody, pivotInWheel, pivotInFork, axisInWheel, axisInFork);

    wm.dynamicsWorld()->addConstraint(axle);
  }

  m_forkBody->setActivationState(DISABLE_DEACTIVATION);
  m_wheelBody->setActivationState(DISABLE_DEACTIVATION);

}

void Unicycle::applyWheelImpulse(double impulse)
{
  if (!m_wheelBody) {
    throw std::runtime_error("Can't add impulse to wheel when it's not initialized.");
  }

  btVector3 impulseLocal(0, 0, impulse);
  btVector3 impulseWorld = m_wheelBody->getWorldTransform().getBasis() * impulseLocal;

  m_wheelBody->applyTorqueImpulse(impulseWorld);
}

void Unicycle::applyForkImpulse(double impulse)
{
  if (!m_forkBody) {
    throw std::runtime_error("Can't add impulse to fork when it's not initialized.");
  }

  btVector3 impulseLocal(0, impulse, 0);
  btVector3 impulseWorld = m_forkBody->getWorldTransform().getBasis() * impulseLocal;

  m_forkBody->applyTorqueImpulse(impulseWorld);
}

void Unicycle::reset(btTransform const& trans)
{
  if (!(m_wheelBody && m_forkBody)) return;

  btTransform forkTrans = trans;
  forkTrans.getOrigin() += btVector3(0, m_forkLength, 0);

  btVector3 zeros(0, 0, 0);

  m_wheelBody->setWorldTransform(trans);
  m_wheelBody->setLinearVelocity(zeros);
  m_wheelBody->setAngularVelocity(zeros);
  m_forkBody->setWorldTransform(forkTrans);
  m_forkBody->setLinearVelocity(zeros);
  m_forkBody->setAngularVelocity(zeros);
}

void Unicycle::updateAngles()
{
  if (!(m_wheelBody && m_forkBody)) return;

  btQuaternion q = m_forkBody->getWorldTransform().getRotation();

  btScalar qw = q.getW();
  btScalar qx = q.getX();
  btScalar qy = q.getY();
  btScalar qz = q.getZ();

  btScalar sqw = qw * qw;
  btScalar sqx = qx * qx;
  btScalar sqy = qy * qy;
  btScalar sqz = qz * qz;

	btScalar test = qx * qy + qz * qw;

	// singularity at north pole
	if (test > 0.499) {
		m_yaw   = 2 * atan2(qx, qw);
		m_pitch = M_PI / 2.0;
		m_roll  = 0;
		return;
	}

	// singularity at south pole
	if (test < -0.499) {
		m_yaw   = -2 * atan2(qx, qw);
		m_pitch = -M_PI / 2;
		m_roll  = 0;
		return;
	}

  m_yaw   = atan2(2 * qy * qw - 2 * qx * qz , sqx - sqy - sqz + sqw);
	m_pitch = asin(2 * test);
  m_roll  = atan2(2 * qx * qw - 2 * qy * qz , -sqx + sqy - sqz + sqw);
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

