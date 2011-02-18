#include <stdexcept>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"

Unicycle::Unicycle()
  : m_forkWidth(0.25)
  , m_forkLength(4.0)
  , m_wheelWidth(0.25)
  , m_wheelRadius(2.0)
  , m_forkIdx(-1)
  , m_wheelIdx(-1)
  , m_forkBody(0)
  , m_wheelBody(0)
{}

void Unicycle::createForkShape(WorldManager& wm)
{
  // Make the unicycle fork
  btCompoundShape* forkShape = new btCompoundShape();
  {
    float totalForkWidth = 2 * (m_forkWidth + m_wheelWidth);
    btCollisionShape* forkLeg = new btBoxShape(btVector3(m_forkWidth/2, m_forkLength/2, m_forkWidth/2));
    btCollisionShape* forkTop = new btBoxShape(btVector3(m_forkWidth/2, m_forkWidth/2, totalForkWidth/2));
    btCollisionShape* seat = new btCapsuleShapeX(btScalar(totalForkWidth * 0.5), btScalar(m_forkLength * 0.2));

    wm.addCollisionShape(forkLeg);
    wm.addCollisionShape(forkTop);
    wm.addCollisionShape(seat);

    btTransform forkLegBTrans, forkTopTrans, seatTrans;
    forkLegBTrans.setIdentity();
    forkTopTrans.setIdentity();
    seatTrans.setIdentity();

    forkLegBTrans.setOrigin(btVector3(0, 0, (totalForkWidth/2 - m_forkWidth/2)));
    forkTopTrans.setOrigin(btVector3(0, m_forkLength/2, 0));
    seatTrans.setOrigin(btVector3(0, (m_forkLength * 1.2)/2, 0));

    forkShape->addChildShape(forkLegBTrans, forkLeg);
    forkShape->addChildShape(forkTopTrans, forkTop);
    forkShape->addChildShape(seatTrans, seat);
  }

  m_forkIdx = wm.addCollisionShape(forkShape);
}

void Unicycle::createWheelShape(WorldManager& wm)
{
  btCollisionShape* wheelShape = new btCylinderShapeZ(btVector3(m_wheelRadius, m_wheelRadius, m_wheelWidth/2));

  m_wheelIdx = wm.addCollisionShape(wheelShape);
}

void Unicycle::addToManager(WorldManager& wm, btTransform const& trans)
{
  if (m_forkIdx != -1 || m_wheelIdx != -1) {
    throw std::runtime_error("Can't add a Unicycle to a WorldManager more than once.");
  }

  createForkShape(wm);
  createWheelShape(wm);

  btTransform forkTrans(trans), wheelTrans(trans);

  wheelTrans.getOrigin() += btVector3(0, -m_forkLength/2, 0);

  m_forkBody  = wm.addRigidBody(0.0, forkTrans, m_forkIdx);
  m_wheelBody = wm.addRigidBody(2.5, wheelTrans, m_wheelIdx);

  m_forkBody->setActivationState(DISABLE_DEACTIVATION);
  m_wheelBody->setActivationState(DISABLE_DEACTIVATION);

  btVector3 pivotInWheel(0, 0, m_wheelWidth);
  btVector3 pivotInFork(0, -m_forkLength/2, m_wheelWidth);
  btVector3 axisInWheel(0, 0, 1);
  btVector3 axisInFork(0, 0, 1);

  btTypedConstraint* axle = new btHingeConstraint(*m_wheelBody, *m_forkBody, pivotInWheel, pivotInFork, axisInWheel, axisInFork);

  wm.dynamicsWorld()->addConstraint(axle);
}

