#include <stdexcept>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"

Unicycle::Unicycle()
  : m_forkWidth(0.25)
  , m_forkLength(4.0)
  , m_wheelWidth(0.25)
  , m_wheelRadius(2.0)
  , m_forkMass(10.0)
  , m_wheelMass(2.5)
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
    float totalForkWidth = 2 * (m_forkWidth + m_wheelWidth);
    btCollisionShape* forkLeg = new btBoxShape(btVector3(m_forkWidth/2, m_forkLength/2, m_forkWidth/2));
    btCollisionShape* forkTop = new btBoxShape(btVector3(m_forkWidth/2, m_forkWidth/2, totalForkWidth/2));
    btCollisionShape* seat    = new btCapsuleShapeX(btScalar(totalForkWidth * 0.5), btScalar(m_forkLength * 0.2));

    wm.addCollisionShape(forkLeg);
    wm.addCollisionShape(forkTop);
    wm.addCollisionShape(seat);

    btTransform forkLegATrans, forkLegBTrans, forkTopTrans, seatTrans;
    forkLegATrans.setIdentity();
    forkLegBTrans.setIdentity();
    forkTopTrans.setIdentity();
    seatTrans.setIdentity();

    forkLegATrans.setOrigin(btVector3(0, 0, -(totalForkWidth/2 - m_forkWidth/2)));
    forkLegBTrans.setOrigin(btVector3(0, 0, (totalForkWidth/2 - m_forkWidth/2)));
    forkTopTrans.setOrigin(btVector3(0, m_forkLength/2, 0));
    seatTrans.setOrigin(btVector3(0, (m_forkLength * 1.2)/2, 0));

    m_forkShape->addChildShape(forkLegATrans, forkLeg);
    m_forkShape->addChildShape(forkLegBTrans, forkLeg);
    m_forkShape->addChildShape(forkTopTrans, forkTop);
    m_forkShape->addChildShape(seatTrans, seat);
  }

  wm.addCollisionShape(m_forkShape);
}

void Unicycle::createForkBody(WorldManager& wm, btTransform const& trans)
{
  btTransform localTrans = trans;
  localTrans.getOrigin() += btVector3(0, m_forkLength, 0);

  btTransform principal;
 	btVector3 principalInertia;
 	btVector3 localInertia;

 	btScalar masses[4] = {0, 0, 0, m_forkMass};

 	m_forkShape->calculatePrincipalAxisTransform(masses, principal, principalInertia);

	btCompoundShape* newForkShape = new btCompoundShape();
	for (int i = 0; i < m_forkShape->getNumChildShapes(); i += 1) {
		newForkShape->addChildShape(principal.inverse() * m_forkShape->getChildTransform(i), m_forkShape->getChildShape(i));
	}
	wm.addCollisionShape(newForkShape);

  m_forkShape = newForkShape;

	btDefaultMotionState* motionState = new btDefaultMotionState(localTrans);

  m_forkBody = new btRigidBody(m_forkMass, motionState, m_forkShape, principalInertia);

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

