#include "unicycle.h"

const btScalar Unicycle::forkWidth   = 0.05;
const btScalar Unicycle::forkLength  = 0.8;
const btScalar Unicycle::wheelWidth  = 0.05;
const btScalar Unicycle::wheelRadius = 0.4;

Unicycle::Unicycle () {
  // Make the unicycle fork
  btCompoundShape* m_forkShape = new btCompoundShape();
  {
    float totalForkWidth = 2 * (forkWidth + wheelWidth);
    btCollisionShape* forkLegf = new btBoxShape(btVector3(forkWidth/2, forkWidth/2, forkLength/2));
    btCollisionShape* forkTop = new btBoxShape(btVector3(totalForkWidth/2, forkWidth/2, forkWidth/2));
    btCollisionShape* seat = new btCapsuleShape(btScalar(totalForkWidth * 0.5), btScalar(forkLength * 0.2));

    m_collisionShapes.push_back(forkLeg);
    m_collisionShapes.push_back(forkTop);
    m_collisionShapes.push_back(seat);

    btTransform forkLegATrans, forkLegBTrans, forkTopTrans, seatTrans;
    forkLegATrans.setIdentity();
    forkLegBTrans.setIdentity();
    forkTopTrans.setIdentity();
    seatTrans.setIdentity();

    forkLegATrans.setOrigin(btVector3(-(totalForkWidth/2 - fork[0]/2), 0, 0));
    forkLegBTrans.setOrigin(btVector3( (totalForkWidth/2 - fork[0]/2), 0, 0));
    forkTopTrans.setOrigin(btVector3(0, 0, fork[1]/2 - fork[0]/2));
    seatTrans.setOrigin(btVector3(0, 0, (fork[1] * 1.2)/2));

    forkShape->addChildShape(forkLegATrans, forkLeg);
    forkShape->addChildShape(forkLegBTrans, forkLeg);
    forkShape->addChildShape(forkTopTrans, forkTop);
    forkShape->addChildShape(seatTrans, seat);
  }

  // Make the unicycle wheel

  btCollisionShape* wheelShape = new btCylinderShapeX(btVector3(wheel[0]/2, wheel[1], wheel[1]));

  m_collisionShapes.push_back(forkShape);
  m_collisionShapes.push_back(wheelShape);

  btTransform forkTrans, wheelTrans;
  forkTrans.setIdentity();
  wheelTrans.setIdentity();

  forkTrans.setOrigin(pos);
  wheelTrans.setOrigin(pos + btVector3(0, 0, -fork[1]/2));

  btRigidBody* forkBody  = localCreateRigidBody(1, forkTrans, forkShape);
  btRigidBody* wheelBody = localCreateRigidBody(0.5, wheelTrans, wheelShape);

  btVector3 pivotInWheel1(wheel[0], 0, 0);
  btVector3 pivotInFork1(wheel[0], 0, -fork[1]/2);
  btVector3 pivotInWheel2(-wheel[0], 0, 0);
  btVector3 pivotInFork2(-wheel[0], 0, -fork[1]/2);
  btVector3 axisInWheel(1, 0, 0);
  btVector3 axisInFork(1, 0, 0);

  btTypedConstraint* hinge1 = new btHingeConstraint(*wheelBody, *forkBody, pivotInWheel1, pivotInFork1, axisInWheel, axisInFork);
  btTypedConstraint* hinge2 = new btHingeConstraint(*wheelBody, *forkBody, pivotInWheel1, pivotInFork1, axisInWheel, axisInFork);

  m_dynamicsWorld->addConstraint(hinge1);
  m_dynamicsWorld->addConstraint(hinge2);
}
