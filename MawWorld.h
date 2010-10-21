#ifndef MAW_WORLD_H
#define MAW_WORLD_H

#include <BulletDynamics/btBulletDynamicsCommon.h>

class MawWorld {
public:
  void initPhysics();
	void endPhysics();
  
  void stepSimulation();
  
  MawWorld()
  : m_collisionConfiguration()
  , m_dispatcher()
  , m_broadphase()
  , m_solver()
  , m_dynamicsWorld()
  , m_collisionShapes()
  {};
  
  MawWorld(MawWorld const&);
  MawWorld& operator=(MawWorld const&);
  
  ~MawWorld() { endPhysics(); }

private:
  btDefaultCollisionConfiguration*     m_collisionConfiguration;
  btCollisionDispatcher*               m_dispatcher;
  btBroadphaseInterface*               m_broadphase;
  btSequentialImpulseConstraintSolver* m_solver;
  
	btDiscreteDynamicsWorld*             m_dynamicsWorld;
  
  btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif // MAW_WORLD_H