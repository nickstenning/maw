#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "vendor/BulletGL/GlutDemoApplication.h"
#include "vendor/BulletGL/GlutRunner.h"

#include "maw_world_manager.h"

int main (int argc, char** argv)
{
  MawWorldManager maw;
  GlutDemoApplication app;

  // Point app at MAW dynamicsWorld;
  app.dynamicsWorld(maw.dynamicsWorld());

  btTransform t(btQuaternion::getIdentity());

  t.getOrigin() += btVector3(0, 4.0, 0);

  maw.addUnicycle(t);

  t.getOrigin() += btVector3(4.0, 0, 0);

  maw.addUnicycle(t);

  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
