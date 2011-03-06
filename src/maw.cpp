#include <iostream>
#include <iomanip>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "vendor/BulletGL/GlutDemoApplication.h"
#include "vendor/BulletGL/GlutRunner.h"

#include "world_manager.h"
#include "util.h"
#include "unicycle.h"

WorldManager wm;
Unicycle uni;

#define SCORE_ANG 1

void handleKeyboardEvent (unsigned char key, int, int)
{
  switch (key) {
    case 't': uni.applyWheelImpulse(10.0);  break;
    case 'y': uni.applyWheelImpulse(-10.0); break;

    case 'g': uni.applyForkImpulse(10.0);  break;
    case 'h': uni.applyForkImpulse(-10.0); break;

    case 'm': uni.translate(util::rand(-5, 5), 0, 0); break;
    case ',': uni.translate(0, util::rand(-5, 5), 0); break;
    case '.': uni.translate(0, 0, util::rand(-5, 5)); break;

    case 'j': uni.resetAxis(); break;

    case ' ': uni.reset(); break;

    default: break;
  }
}

void simulationCallback ()
{
  uni.updateAngles();
}

int main (int argc, char** argv)
{
  std::cout << std::setprecision(6) << std::fixed;
  std::cerr << std::setprecision(6) << std::fixed;

  btQuaternion q = btQuaternion::getIdentity();

  uni.addToManager(wm);

  GlutDemoApplication app;

  app.dynamicsWorld(wm.dynamicsWorld());
  app.idle(true);
  app.registerKeyHandler(handleKeyboardEvent);
  app.registerStepCallback(simulationCallback);

  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
