#ifndef MAW_WORLD_H
#define MAW_WORLD_H

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "GlutDemoApplication.h"

class MawWorld : public GlutDemoApplication {
public:
  void initPhysics();
	void endPhysics();

  void stepSimulation(float dt);

  MawWorld()
  : m_collisionConfiguration(0)
  , m_dispatcher(0)
  , m_broadphase(0)
  , m_solver(0)
  , m_collisionShapes()
  {};

  MawWorld(MawWorld const&);
  MawWorld& operator=(MawWorld const&);

  ~MawWorld() { endPhysics(); }

  void setDebugDrawer(btIDebugDraw& drawer);
  virtual void clientMoveAndDisplay();
  virtual void displayCallback();
  virtual void keyboardCallback(unsigned char key, int x, int y);

private:
  btDefaultCollisionConfiguration*     m_collisionConfiguration;
  btCollisionDispatcher*               m_dispatcher;
  btBroadphaseInterface*               m_broadphase;
  btSequentialImpulseConstraintSolver* m_solver;

  btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif // MAW_WORLD_H