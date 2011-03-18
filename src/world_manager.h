#ifndef WORLD_MANAGER_H
#define WORLD_MANAGER_H

#include <LinearMath/btAlignedObjectArray.h>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDynamicsWorld;
class btCollisionShape;

class WorldManager
{
public:
  WorldManager();
  virtual ~WorldManager();

  btDynamicsWorld* dynamics_world() const;

  btRigidBody* add_rigid_body(btScalar mass, const btTransform& transform, btCollisionShape* shape);

  btCollisionShape* add_collision_shape(btCollisionShape* shape);

  int step_simulation(btScalar timeStep);

protected:
  btAlignedObjectArray<btCollisionShape*> m_collision_shapes;

private:
  // Uncopyable
  WorldManager(WorldManager const&);
  WorldManager& operator=(WorldManager const&);

  btDefaultCollisionConfiguration*     m_collision_conf;
  btCollisionDispatcher*               m_dispatcher;
  btBroadphaseInterface*               m_broadphase;
  btSequentialImpulseConstraintSolver* m_solver;
  btDynamicsWorld*                     m_dynamics_world;
};

std::ostream& operator<<(std::ostream& os, btVector3 const& v);

#endif // WORLD_MANAGER_H
