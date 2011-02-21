#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "vendor/BulletGL/GlutDemoApplication.h"
#include "vendor/BulletGL/GlutRunner.h"

#include "world_manager.h"
#include "unicycle.h"

WorldManager wm;
Unicycle uni;

btTransform startTransform;

void handleKeyboardEvent (unsigned char key, int, int)
{
  switch (key) {
    case 't': uni.applyWheelImpulse(10.0);  break;
    case 'y': uni.applyWheelImpulse(-10.0); break;

    case ' ': uni.reset(startTransform); break;

    default: break;
  }
}

int main (int argc, char** argv)
{
  startTransform.setIdentity();
  startTransform.setOrigin(btVector3(0, 2.01, 0));
  uni.addToManager(wm, startTransform);

  GlutDemoApplication app;

  app.dynamicsWorld(wm.dynamicsWorld());
  app.registerKeyHandler(handleKeyboardEvent);

  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
