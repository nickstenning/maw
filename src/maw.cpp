#include <iostream>
#include <iomanip>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "vendor/BulletGL/GlutDemoApplication.h"
#include "vendor/BulletGL/GlutRunner.h"

#include "nn.h"
#include "world_manager.h"
#include "util.h"
#include "unicycle.h"

WorldManager wm;
Unicycle uni;
NN nn;

void handleKeyboardEvent (unsigned char key, int, int)
{
  switch (key) {
    case 't': uni.applyWheelImpulse(10.0);  break;
    case 'y': uni.applyWheelImpulse(-10.0); break;

    case 'g': uni.applyForkImpulse(10.0);  break;
    case 'h': uni.applyForkImpulse(-10.0); break;

    case ' ': uni.reset(); break;

    default: break;
  }
}

void simulationCallback ()
{
  uni.computeState();

  std::vector<double> input;
  std::vector<int> output;

  std::cout << uni.yaw() << "\n";

  input.push_back(uni.yaw());
  input.push_back(uni.pitch());
  input.push_back(uni.roll());
  input.push_back(uni.wheelVelocity());
  input.push_back(0.5);

  output = nn.feedForward(input);

  double yawImpulse = 2.0 * output[0];
  double pitchImpulse = 2.0 * output[1];

  uni.applyForkImpulse(yawImpulse);
  uni.applyWheelImpulse(pitchImpulse);

}

int main (int argc, char** argv)
{
  std::cin >> nn;

  std::cout << std::setprecision(6) << std::fixed;
  std::cerr << std::setprecision(6) << std::fixed;

  uni.addToManager(wm);

  GlutDemoApplication app;

  app.dynamicsWorld(wm.dynamicsWorld());
  app.registerKeyHandler(handleKeyboardEvent);
  app.registerStepCallback(simulationCallback);

  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
