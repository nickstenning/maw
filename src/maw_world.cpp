#include "vendor/BulletGL/GLDebugDrawer.h"

#include "maw_world.h"

MawWorld::MawWorld ()
: GlutDemoApplication()
, m_debugDrawer(new GLDebugDrawer())
, m_collisionConfiguration(0)
, m_dispatcher(0)
, m_broadphase(0)
, m_solver(0)
, m_collisionShapes()
{
  // setTexturing(true);
  // setShadows(true);
}

void MawWorld::initPhysics() {
  // Set up camera
  m_sundirection = btVector3(1,1,-2) * 1000;
  m_cameraUp = btVector3(0,0,1);
  m_ele = 150;
  m_azi = 200;
  m_forwardAxis = 1;
  m_cameraDistance = 5.0;

  // Create dynamicsWorld
  m_collisionConfiguration = new btDefaultCollisionConfiguration();
  m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
  m_broadphase = new btDbvtBroadphase();
  m_solver     = new btSequentialImpulseConstraintSolver();

  m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

  m_dynamicsWorld->setDebugDrawer(m_debugDrawer);
  m_dynamicsWorld->setGravity(btVector3(0, 0, -10));

  // Create ground
  btCollisionShape* groundShape = new btBoxShape(btVector3(10,10,1));
  m_collisionShapes.push_back(groundShape);

  btTransform groundTransform;
  groundTransform.setIdentity();
  groundTransform.setOrigin(btVector3(0,0,-1));

  localCreateRigidBody(0.0, groundTransform, groundShape);

  addUnicycle(btVector3(0, 0, 0.8));
}

void MawWorld::stepSimulation(float dt) {
  m_dynamicsWorld->stepSimulation(dt);
}

void MawWorld::endPhysics() {
  // remove the rigid bodies from the dynamics world and delete them
  for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i -= 1) {
    btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(obj);

    if (body && body->getMotionState()) {
      // remove constraints
      while (body->getNumConstraintRefs()) {
        btTypedConstraint* constraint = body->getConstraintRef(0);
        m_dynamicsWorld->removeConstraint(constraint);
        delete constraint;
      }

      delete body->getMotionState();
    }

    m_dynamicsWorld->removeCollisionObject( obj );
    delete obj;
  }

  // delete collision shapes
  for (int j = 0; j < m_collisionShapes.size();j++) {
    btCollisionShape* shape = m_collisionShapes[j];
    m_collisionShapes[j] = 0;
    delete shape;
  }
  m_collisionShapes.clear();

  delete m_debugDrawer;
  delete m_dynamicsWorld;

  delete m_solver;
  delete m_broadphase;
  delete m_dispatcher;
  delete m_collisionConfiguration;
}


void MawWorld::clientMoveAndDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float dt = getDeltaTimeMicroseconds() * 0.000001f;

  stepSimulation(dt);

  if (m_dynamicsWorld) {
    m_dynamicsWorld->debugDrawWorld();
  }

  renderme();

  glFlush();
  glutSwapBuffers();
}

void MawWorld::displayCallback(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_dynamicsWorld) {
    m_dynamicsWorld->debugDrawWorld();
  }

  renderme();

  glFlush();
  glutSwapBuffers();
}

void MawWorld::addUnicycle(btVector3 const& pos) {
  const btScalar forkWidth   = 0.05;
  const btScalar forkLength  = 0.8;
  const btScalar wheelWidth  = 0.05;
  const btScalar wheelRadius = 0.4;

  // Make the unicycle fork
  btCompoundShape* forkShape = new btCompoundShape();
  {
    float totalForkWidth = 2 * (forkWidth + wheelWidth);
    btCollisionShape* forkLeg = new btBoxShape(btVector3(forkWidth/2, forkWidth/2, forkLength/2));
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

    forkLegATrans.setOrigin(btVector3(-(totalForkWidth/2 - forkWidth/2), 0, 0));
    forkLegBTrans.setOrigin(btVector3( (totalForkWidth/2 - forkWidth/2), 0, 0));
    forkTopTrans.setOrigin(btVector3(0, 0, forkLength/2 - forkLength/2));
    seatTrans.setOrigin(btVector3(0, 0, (forkLength * 1.2)/2));

    forkShape->addChildShape(forkLegATrans, forkLeg);
    forkShape->addChildShape(forkLegBTrans, forkLeg);
    forkShape->addChildShape(forkTopTrans, forkTop);
    forkShape->addChildShape(seatTrans, seat);
  }

  // Make the unicycle wheel

  btCollisionShape* wheelShape = new btCylinderShapeX(btVector3(wheelWidth/2, wheelRadius, wheelRadius));

  m_collisionShapes.push_back(forkShape);
  m_collisionShapes.push_back(wheelShape);

  btTransform forkTrans, wheelTrans;
  forkTrans.setIdentity();
  wheelTrans.setIdentity();

  forkTrans.setOrigin(pos);
  wheelTrans.setOrigin(pos + btVector3(0, 0, -forkLength/2));

  btRigidBody* forkBody  = localCreateRigidBody(1, forkTrans, forkShape);
  btRigidBody* wheelBody = localCreateRigidBody(0.5, wheelTrans, wheelShape);

  btVector3 pivotInWheel1(wheelWidth, 0, 0);
  btVector3 pivotInFork1(wheelWidth, 0, -forkLength/2);
  btVector3 pivotInWheel2(-wheelWidth, 0, 0);
  btVector3 pivotInFork2(-wheelWidth, 0, -forkLength/2);
  btVector3 axisInWheel(1, 0, 0);
  btVector3 axisInFork(1, 0, 0);

  btTypedConstraint* hinge1 = new btHingeConstraint(*wheelBody, *forkBody, pivotInWheel1, pivotInFork1, axisInWheel, axisInFork);
  btTypedConstraint* hinge2 = new btHingeConstraint(*wheelBody, *forkBody, pivotInWheel1, pivotInFork1, axisInWheel, axisInFork);

  m_dynamicsWorld->addConstraint(hinge1);
  m_dynamicsWorld->addConstraint(hinge2);
}

