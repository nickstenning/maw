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

void handle_key_event (unsigned char key, int, int)
{
  switch (key) {
    case 't': uni.apply_wheel_impulse( 10.0, true);  break;
    case 'y': uni.apply_wheel_impulse(-10.0, true); break;

    case 'g': uni.apply_fork_impulse( 10.0, true);  break;
    case 'h': uni.apply_fork_impulse(-10.0, true); break;

    case ' ': uni.reset(); break;

    default: break;
  }
}

void simulation_callback ()
{
  uni.compute_state();

  std::vector<double> input;
  std::vector<int> output;

  input.push_back(uni.yaw());
  input.push_back(uni.pitch());
  input.push_back(uni.roll());
  input.push_back(uni.wheel_velocity());
  input.push_back(uni.yaw_velocity());

  output = nn.feed(input);

  uni.apply_fork_impulse(output[0]);
  uni.apply_wheel_impulse(output[1]);
}

int main (int argc, char** argv)
{
  std::cin >> nn;

  std::cout << std::setprecision(6) << std::fixed;
  std::cerr << std::setprecision(6) << std::fixed;

  uni.add_to_manager(wm);
  uni.reset();

  GlutDemoApplication app;

  app.dynamics_world(wm.dynamics_world());
  app.register_key_handler(handle_key_event);
  app.register_step_callback(simulation_callback);

  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
