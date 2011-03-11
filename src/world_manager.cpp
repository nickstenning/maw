#include <stdexcept>
#include <iostream>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"

WorldManager::WorldManager ()
  : m_collisionShapes()
  , m_collisionConf(new btDefaultCollisionConfiguration())
  , m_dispatcher(new btCollisionDispatcher(m_collisionConf))
  , m_broadphase(new btDbvtBroadphase())
  , m_solver(new btSequentialImpulseConstraintSolver())
  , m_dynamicsWorld(new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConf))
{
  m_dynamicsWorld->setGravity(btVector3(0, -10, 0));

  // Create ground
  {
    btCollisionShape* groundShape = addCollisionShape(new btBoxShape(btVector3(50, 1, 50)));

    btTransform groundTransform(btQuaternion::getIdentity(), btVector3(0, -1, 0));

    addRigidBody(btScalar(0.0), groundTransform, groundShape);
  }
}

WorldManager::~WorldManager()
{
  // remove the rigid bodies from the dynamics world and delete them
  for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i -= 1) {
    btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(obj);

    // remove constraints
    while (body && body->getNumConstraintRefs()) {
      btTypedConstraint* constraint = body->getConstraintRef(0);
      m_dynamicsWorld->removeConstraint(constraint);
      delete constraint;
    }

    m_dynamicsWorld->removeCollisionObject(obj);
    delete obj;
  }

  // delete collision shapes
  for (int j = 0; j < m_collisionShapes.size(); j++) {
    btCollisionShape* shape = m_collisionShapes[j];
    m_collisionShapes[j] = 0;
    delete shape;
  }
  m_collisionShapes.clear();

  delete m_dynamicsWorld;

  delete m_solver;
  delete m_broadphase;
  delete m_dispatcher;
  delete m_collisionConf;
}

btDynamicsWorld* WorldManager::dynamicsWorld() const
{
  return m_dynamicsWorld;
}

btRigidBody* WorldManager::addRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape)
{
  btAssert(!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE);

  // dynamic iff mass is nonzero, otherwise static.
  bool isDynamic = (mass != 0.0);

  btVector3 localInertia(0, 0, 0);
  if (isDynamic) {
    shape->calculateLocalInertia(mass, localInertia);
  }

 	btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, 0, shape, localInertia);
  bodyCI.m_startWorldTransform = startTransform;
  btRigidBody* body = new btRigidBody(bodyCI);

  m_dynamicsWorld->addRigidBody(body);

  return body;
}

btCollisionShape* WorldManager::addCollisionShape(btCollisionShape* shape)
{
  m_collisionShapes.push_back(shape);
  return shape;
}

int WorldManager::stepSimulation(btScalar timeStep)
{
  return m_dynamicsWorld->stepSimulation(timeStep);
}

std::ostream& operator<<(std::ostream& os, btVector3 const& v)
{
  os << "[ x = " << v.getX() << ",\ty = " << v.getY() << ",\tz = " << v.getZ() << " ]";
  return os;
}
