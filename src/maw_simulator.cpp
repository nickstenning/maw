#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "vendor/BulletGL/GlutDemoApplication.h"
#include "vendor/BulletGL/GlutRunner.h"

#include "nn.h"
#include "world_manager.h"
#include "util.h"
#include "unicycle.h"
#include "maw_simulator.h"

static WorldManager wm;
static Unicycle* uni;
static NN* nn;

static void handle_key_event (unsigned char key, int, int)
{
  switch (key) {
    case 't': uni->apply_wheel_impulse( 1.0); break;
    case 'y': uni->apply_wheel_impulse(-1.0); break;

    case 'g': uni->apply_drive_impulse( 1.0); break;
    case 'h': uni->apply_drive_impulse(-1.0); break;

    case ' ': uni->reset(); break;
    case '.': uni->reset(Unicycle::reset_transform, true); break;

    default: break;
  }
}

static void simulation_callback ()
{
  uni->compute_state();

  std::vector<double> input;
  std::vector<int> output;

  input.push_back(uni->yaw());
  input.push_back(uni->pitch());
  input.push_back(uni->roll());
  input.push_back(uni->wheel_velocity());
  input.push_back(uni->yaw_velocity());

  output = nn->feed(input);

  uni->apply_drive_impulse(output[0]);
  uni->apply_wheel_impulse(output[1]);
}

int simulate (Unicycle& user_uni, NN& user_nn)
{
  int argc = 1;
  char* argv[1] = {"maw_simulator"};

  uni = &user_uni;
  nn = &user_nn;

  uni->add_to_manager(wm);
  uni->reset();

  GlutDemoApplication app;

  app.dynamics_world(wm.dynamics_world());
  app.register_key_handler(handle_key_event);
  app.register_step_callback(simulation_callback);

  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
