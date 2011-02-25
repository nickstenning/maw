#include <stdexcept>
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

	btDefaultMotionState* motionState = new btDefaultMotionState(trans);
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

  if (m_wheelBody) {
    btTransform wheelTrans = trans;
    wheelTrans.getOrigin() += btVector3(0, m_wheelRadius, 0);

    m_wheelBody->setWorldTransform(wheelTrans);
    m_wheelBody->setLinearVelocity(zeros);
    m_wheelBody->setAngularVelocity(zeros);
  }

  if (m_forkBody) {
    btTransform forkTrans = trans;
    forkTrans.getOrigin() += btVector3(0, m_wheelRadius + m_forkLength, 0);

    m_forkBody->setWorldTransform(forkTrans);
    m_forkBody->setLinearVelocity(zeros);
    m_forkBody->setAngularVelocity(zeros);
  }
}

void computeEulerAngles(btQuaternion const& quat, btScalar& yaw, btScalar& pitch, btScalar& roll, btQuaternion const basis = btQuaternion::getIdentity())
{
  const btQuaternion q(quat - basis);

  const btScalar qw = q.getW();
  const btScalar qx = q.getX();
  const btScalar qy = q.getY();
  const btScalar qz = q.getZ();

  const btScalar sqw = qw * qw;
  const btScalar sqx = qx * qx;
  const btScalar sqy = qy * qy;
  const btScalar sqz = qz * qz;

	const btScalar test = qx * qy + qz * qw;
	const btScalar unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor

	// singularity at north pole
	if (test > 0.499 * unit) {
		yaw   = 2 * atan2(qx, qw);
		pitch = M_PI / 2.0;
		roll  = 0;

	// singularity at south pole
	} else if (test < -0.499 * unit) {
		yaw   = -2 * atan2(qx, qw);
		pitch = -M_PI / 2;
		roll  = 0;

  } else {
    yaw   = atan2(2 * qy * qw - 2 * qx * qz , sqx - sqy - sqz + sqw);
  	pitch = asin(2 * test / unit);
    roll  = atan2(2 * qx * qw - 2 * qy * qz , -sqx + sqy - sqz + sqw);
  }
}

void Unicycle::updateAngles()
{
  // if (!(m_forkBody && m_wheelBody)) return;

  btQuaternion wheelQuat = m_wheelBody->getWorldTransform().getRotation();
  // btQuaternion forkQuat = m_forkBody->getWorldTransform().getRotation();

  btScalar wheelYaw, wheelPitch, wheelRoll;
  // btScalar forkYaw, forkPitch, forkRoll;

  // Create basis rotated such that x-axis in basis is aligned with wheel axis.
  btQuaternion wheelAlignedWorldBasis(0, M_PI, 0);

  computeEulerAngles(wheelQuat, wheelYaw, wheelPitch, wheelRoll, wheelAlignedWorldBasis);

  // // Create basis rotated such that x-axis in basis is aligned in yaw direction
  // // of wheel.
  // btQuaternion wheelBasis(wheelYaw, M_PI, 0);
  //
  // computeEulerAngles(forkQuat, forkYaw, forkPitch, forkRoll, wheelBasis);

  // The angles we are interested in are wheelYaw, forkPitch, forkRoll
  m_yaw   = wheelYaw;
  // m_pitch = forkPitch;
  // m_roll  = forkRoll;
}

void Unicycle::setEuler(btScalar y, btScalar p, btScalar r)
{
  btQuaternion q;
  q.setEuler(y, p, r);

  btTransform t = resetTransform;
  t.setRotation(q);

  reset(t);
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

Unicycle& Unicycle::yaw(btScalar y)
{
  btQuaternion wheelQuat = m_wheelBody->getWorldTransform().getRotation();

  btScalar y_old, p, r;
  btQuaternion wheelAlignedWorldBasis(0, M_PI, 0);

  computeEulerAngles(wheelQuat, y_old, p, r, wheelAlignedWorldBasis);

  btQuaternion q;
  q.setEuler(y, 0, 0);

  m_wheelBody->getWorldTransform().setRotation(q);

  return *this;
}

Unicycle& Unicycle::pitch(btScalar p)
{
  updateAngles();
  setEuler(m_yaw, p, m_roll);

  return *this;
}

Unicycle& Unicycle::roll(btScalar r)
{
  updateAngles();
  setEuler(m_yaw, m_pitch, r);

  return *this;
}

