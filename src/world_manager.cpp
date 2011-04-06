#include <stdexcept>
#include <iostream>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"

WorldManager::WorldManager (bool is_graphical)
  : m_collision_shapes()
  , m_collision_conf(new btDefaultCollisionConfiguration())
  , m_dispatcher(new btCollisionDispatcher(m_collision_conf))
  , m_broadphase(new btDbvtBroadphase())
  , m_solver(new btSequentialImpulseConstraintSolver())
  , m_dynamics_world(new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collision_conf))
{
  m_dynamics_world->setGravity(btVector3(0, -10, 0));

  // Create ground. Use btStaticPlaneShape in GA evaluation to avoid issues
  // involving unicycle falling off plane!
  {
    btCollisionShape* groundShape;
    btTransform groundTransform;

    groundTransform.setIdentity();

    if (is_graphical) {
      groundShape = add_collision_shape(new btBoxShape(btVector3(100, 1, 100)));
      groundTransform.getOrigin() += btVector3(0, -1, 0);
    } else {
      groundShape = add_collision_shape(new btStaticPlaneShape(btVector3(0, 1, 0), 0));
    }

    add_rigid_body(0.0, groundTransform, groundShape);
  }
}

WorldManager::~WorldManager()
{
  // remove the rigid bodies from the dynamics world and delete them
  for (int i = m_dynamics_world->getNumCollisionObjects() - 1; i >= 0; i -= 1) {
    btCollisionObject* obj = m_dynamics_world->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(obj);

    // remove constraints
    while (body && body->getNumConstraintRefs()) {
      btTypedConstraint* constraint = body->getConstraintRef(0);
      m_dynamics_world->removeConstraint(constraint);
      delete constraint;
    }

    m_dynamics_world->removeCollisionObject(obj);
    delete obj;
  }

  // delete collision shapes
  for (int j = 0; j < m_collision_shapes.size(); j++) {
    btCollisionShape* shape = m_collision_shapes[j];
    m_collision_shapes[j] = 0;
    delete shape;
  }
  m_collision_shapes.clear();

  delete m_dynamics_world;

  delete m_solver;
  delete m_broadphase;
  delete m_dispatcher;
  delete m_collision_conf;
}

btDynamicsWorld* WorldManager::dynamics_world() const
{
  return m_dynamics_world;
}

btRigidBody* WorldManager::add_rigid_body(btScalar mass, const btTransform& transform, btCollisionShape* shape)
{
  btAssert(!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE);

  // dynamic iff mass is nonzero, otherwise static.
  bool is_dynamic = (mass != 0.0);

  btVector3 local_inertia(0, 0, 0);
  if (is_dynamic) {
    shape->calculateLocalInertia(mass, local_inertia);
  }

  btRigidBody::btRigidBodyConstructionInfo body_CI(mass, 0, shape, local_inertia);
  body_CI.m_startWorldTransform = transform;
  btRigidBody* body = new btRigidBody(body_CI);

  m_dynamics_world->addRigidBody(body);

  return body;
}

btCollisionShape* WorldManager::add_collision_shape(btCollisionShape* shape)
{
  m_collision_shapes.push_back(shape);
  return shape;
}

int WorldManager::step_simulation(btScalar timeStep)
{
  return m_dynamics_world->stepSimulation(timeStep);
}

std::ostream& operator<<(std::ostream& os, btVector3 const& v)
{
  os << "[ x = " << v.getX() << ",\ty = " << v.getY() << ",\tz = " << v.getZ() << " ]";
  return os;
}
