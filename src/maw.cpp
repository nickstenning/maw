#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "vendor/BulletGL/GlutDemoApplication.h"
#include "vendor/BulletGL/GlutRunner.h"

#include "world_manager.h"
#include "unicycle.h"

int main (int argc, char** argv)
{
  WorldManager wm;
  Unicycle u1, u2;

  btTransform t(btQuaternion::getIdentity());

  t.getOrigin() += btVector3(0, 4.0, 0);

  u1.addToManager(wm, t);

  t.getOrigin() += btVector3(4.0, 0, 0);

  u2.addToManager(wm, t);

  GlutDemoApplication app;
  // Point app at MAW dynamicsWorld;
  app.dynamicsWorld(wm.dynamicsWorld());
  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
