#include "MawWorld.h"
#include <iostream>

void MawWorld::initPhysics() {
  setTexturing(true);
  setShadows(false);

  m_cameraUp = btVector3(0,0,1);
  m_ele = 120;
  m_forwardAxis = 1;

  m_collisionConfiguration = new btDefaultCollisionConfiguration();
  m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
  m_broadphase = new btDbvtBroadphase();
  m_solver     = new btSequentialImpulseConstraintSolver();

  m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

  m_dynamicsWorld->setGravity(btVector3(0, 0, -10));

  // Create ground.
  btCollisionShape* groundShape = new btBoxShape(btVector3(100., 100., 1.));
  m_collisionShapes.push_back(groundShape);

  btTransform groundTransform;
  groundTransform.setIdentity();
  groundTransform.setOrigin(btVector3(0,0,-0.5f));

  btRigidBody* ground = localCreateRigidBody(0.f, groundTransform, groundShape);
  ground->setRestitution(0.9f);

  // Create sphere

  btCollisionShape* ballShape = new btSphereShape(btScalar(1.));
  m_collisionShapes.push_back(ballShape);

  btTransform ballTransform;
  ballTransform.setIdentity();
  ballTransform.setOrigin(btVector3(0,0,10));

  btRigidBody* ball = localCreateRigidBody(1.f, ballTransform, ballShape);
  ball->setRestitution(0.9f);
}

void MawWorld::stepSimulation(float dt) {
  m_dynamicsWorld->stepSimulation(dt);
}

void MawWorld::endPhysics() {
  // remove the rigidbodies from the dynamics world and delete them
  for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i -= 1) {
    btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(obj);
    if (body && body->getMotionState()) {
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

  delete m_dynamicsWorld;

  delete m_solver;
  delete m_broadphase;
  delete m_dispatcher;
  delete m_collisionConfiguration;

  m_collisionShapes.clear();
}


void MawWorld::setDebugDrawer(btIDebugDraw& drawer) {
  m_dynamicsWorld->setDebugDrawer(&drawer);
}

void MawWorld::clientMoveAndDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float dt = getDeltaTimeMicroseconds() * 0.000001f;

  stepSimulation(dt);

  //optional but useful: debug drawing
  m_dynamicsWorld->debugDrawWorld();

  renderme();

  glFlush();
  glutSwapBuffers();

}

void MawWorld::displayCallback(void) {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_dynamicsWorld)
    m_dynamicsWorld->debugDrawWorld();

  renderme();

  glFlush();
  glutSwapBuffers();
}

void MawWorld::keyboardCallback(unsigned char key, int x, int y) {
  if (key == 'a') {
    std::cout << "You pressed an 'a'!\n";
  }
  DemoApplication::keyboardCallback(key, x, y);
}

