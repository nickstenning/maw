#include "MawWorld.h"
#include <iostream>

// No-op
MawWorld::MawWorld(MawWorld const&)
: m_collisionConfiguration ()
, m_dispatcher             ()
, m_broadphase             ()
, m_solver                 ()
, m_dynamicsWorld          ()
, m_collisionShapes        ()
{}

// No-op
MawWorld& MawWorld::operator= (MawWorld const& rhs) {
  return *this;
}

void MawWorld::initPhysics() {
  m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
  m_broadphase = new btDbvtBroadphase();
  m_solver     = new btSequentialImpulseConstraintSolver();
  
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
  
	m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
  
	btCollisionShape* groundShape = new btBoxShape(btVector3(10., 10., 10.));
  
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	m_collisionShapes.push_back(groundShape);
  
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-10,0));
  
  {
    btScalar mass(0.);
    
		// rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
    
		btVector3 localInertia(0,0,0);
		if (isDynamic) {
			groundShape->calculateLocalInertia(mass,localInertia);
    }
    
		// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
    
    body->setRestitution(0.5f);
    
		// add the body to the dynamics world
		m_dynamicsWorld->addRigidBody(body);
	}
  
  
	{
		//create a dynamic rigidbody
    
		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		m_collisionShapes.push_back(colShape);
    
		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
    
		btScalar mass(1.f);
    
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
    
		btVector3 localInertia(0,0,0);
		if (isDynamic) {
			colShape->calculateLocalInertia(mass,localInertia);
    }
    
    startTransform.setOrigin(btVector3(0,10,0));
		
    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    
    body->setRestitution(0.9f);
    
    m_dynamicsWorld->addRigidBody(body);
	}
}

void MawWorld::stepSimulation() {
  m_dynamicsWorld->stepSimulation(1.f/60.f,10);
  
  //print positions of all objects
  for (int j = m_dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j -= 1) {
    btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[j];
    btRigidBody* body = btRigidBody::upcast(obj);
    
    if (body && body->getMotionState()) {
      btTransform trans;
      body->getMotionState()->getWorldTransform(trans);
      printf("%d\t%f\t%f\t%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
    }
  }
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


