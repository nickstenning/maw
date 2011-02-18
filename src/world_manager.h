#ifndef WORLD_MANAGER_H
#define WORLD_MANAGER_H

#include "vendor/bullet.h"

class WorldManager
{
public:
  WorldManager();
  virtual ~WorldManager();

  btDynamicsWorld* dynamicsWorld() const;

  btRigidBody* addRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape);

  int stepSimulation(btScalar timeStep);

protected:
  btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

private:
  // Uncopyable
  WorldManager(WorldManager const&);
  WorldManager& operator=(WorldManager const&);

  btDefaultCollisionConfiguration*     m_collisionConf;
  btCollisionDispatcher*               m_dispatcher;
  btBroadphaseInterface*               m_broadphase;
  btSequentialImpulseConstraintSolver* m_solver;
  btDynamicsWorld*                     m_dynamicsWorld;
};

#endif // WORLD_MANAGER_H
