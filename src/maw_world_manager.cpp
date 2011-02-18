#include "maw_world_manager.h"

MawWorldManager::MawWorldManager ()
  : WorldManager()
{}

void MawWorldManager::addUnicycle(btTransform const& trans) {
  const btScalar forkWidth   = 0.25;
  const btScalar forkLength  = 4.0;
  const btScalar wheelWidth  = 0.25;
  const btScalar wheelRadius = 2.0;

  // Make the unicycle fork
  btCompoundShape* forkShape = new btCompoundShape();
  {
    float totalForkWidth = 2 * (forkWidth + wheelWidth);
    btCollisionShape* forkLeg = new btBoxShape(btVector3(forkWidth/2, forkLength/2, forkWidth/2));
    btCollisionShape* forkTop = new btBoxShape(btVector3(forkWidth/2, forkWidth/2, totalForkWidth/2));
    btCollisionShape* seat = new btCapsuleShapeX(btScalar(totalForkWidth * 0.5), btScalar(forkLength * 0.2));

    m_collisionShapes.push_back(forkLeg);
    m_collisionShapes.push_back(forkTop);
    m_collisionShapes.push_back(seat);

    // btTransform forkLegATrans, forkLegBTrans, forkTopTrans, seatTrans;
    btTransform forkLegBTrans, forkTopTrans, seatTrans;
    // forkLegATrans.setIdentity();
    forkLegBTrans.setIdentity();
    forkTopTrans.setIdentity();
    seatTrans.setIdentity();

    // forkLegATrans.setOrigin(btVector3(-(totalForkWidth/2 - forkWidth/2), 0, 0));
    forkLegBTrans.setOrigin(btVector3(0, 0, (totalForkWidth/2 - forkWidth/2)));
    forkTopTrans.setOrigin(btVector3(0, forkLength/2, 0));
    seatTrans.setOrigin(btVector3(0, (forkLength * 1.2)/2, 0));

    // forkShape->addChildShape(forkLegATrans, forkLeg);
    forkShape->addChildShape(forkLegBTrans, forkLeg);
    forkShape->addChildShape(forkTopTrans, forkTop);
    forkShape->addChildShape(seatTrans, seat);
  }

  // Make the unicycle wheel
  btCollisionShape* wheelShape = new btCylinderShapeZ(btVector3(wheelRadius, wheelRadius, wheelWidth/2));

  m_collisionShapes.push_back(forkShape);
  m_collisionShapes.push_back(wheelShape);

  btTransform forkTrans(trans), wheelTrans(trans);

  wheelTrans.getOrigin() += btVector3(0, -forkLength/2, 0);

  btRigidBody* forkBody  = addRigidBody(10.0, forkTrans, forkShape);
  btRigidBody* wheelBody = addRigidBody(2.5, wheelTrans, wheelShape);

  forkBody->setActivationState(DISABLE_DEACTIVATION);
  wheelBody->setActivationState(DISABLE_DEACTIVATION);

  btVector3 pivotInWheel(0, 0, wheelWidth);
  btVector3 pivotInFork(0, -forkLength/2, wheelWidth);
  btVector3 axisInWheel(0, 0, 1);
  btVector3 axisInFork(0, 0, 1);

  btTypedConstraint* axle = new btHingeConstraint(*wheelBody, *forkBody, pivotInWheel, pivotInFork, axisInWheel, axisInFork);

  dynamicsWorld()->addConstraint(axle);
}

