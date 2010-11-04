#ifndef MAW_WORLD_H
#define MAW_WORLD_H

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "GLDebugDrawer.h"
#include "GlutDemoApplication.h"

class MawWorld : public GlutDemoApplication {
public:
  void initPhysics();
	void endPhysics();

  void stepSimulation(float dt);

  MawWorld();
  ~MawWorld() { endPhysics(); }

  virtual void clientMoveAndDisplay();
  virtual void displayCallback();

  void addUnicycle(btVector3 const& pos);

private:
  // Uncopyable
  MawWorld(MawWorld const&);
  MawWorld& operator=(MawWorld const&);

  btIDebugDraw* m_debugDrawer;

  btDefaultCollisionConfiguration*     m_collisionConfiguration;
  btCollisionDispatcher*               m_dispatcher;
  btBroadphaseInterface*               m_broadphase;
  btSequentialImpulseConstraintSolver* m_solver;

  btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif // MAW_WORLD_H