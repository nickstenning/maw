#include <iostream>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "vendor/BulletGL/GlutDemoApplication.h"
#include "vendor/BulletGL/GlutRunner.h"

#include "nn.h"
#include "world_manager.h"
#include "util.h"
#include "unicycle.h"
#include "maw_simulator.h"

static WorldManager wm(true);
static GlutDemoApplication app;
static Unicycle* uni;
static NN* nn;

static double sim_time = 0.0;
static const double dt = app.getDeltaTimeMicroseconds() / 1e6;

static double wheel_impulse = 1.0;
static double drive_impulse = 1.0;

static void handle_key_event (unsigned char key, int, int)
{
  switch (key) {
    case 't': uni->apply_drive_impulse( 3.0 * drive_impulse); break;
    case 'y': uni->apply_drive_impulse(-3.0 * drive_impulse); break;

    case 'g': uni->apply_wheel_impulse( 3.0 * wheel_impulse); break;
    case 'h': uni->apply_wheel_impulse(-3.0 * wheel_impulse); break;

    case ' ': uni->reset(); break;
    case '.': uni->reset(0.1); break;

    default: break;
  }
}

static void simulation_callback ()
{
  sim_time += dt;

  uni->compute_state(dt);

  std::cout << sim_time << "\t"
            << uni->yaw() << "\t" << uni->pitch() << "\t" << uni->roll() << "\t"
            << uni->yaw_velocity() << "\t" << uni->pitch_velocity() << "\t" << uni->roll_velocity() << "\t"
            << uni->wheel_velocity() << "\t"
            << uni->kinetic_energy() << "\t" << uni->potential_energy() << "\t";

  app.cameraTargetPosition(uni->origin());

  bool attempt_control = abs(uni->pitch()) < M_PI / 4.0 && abs(uni->roll()) < M_PI / 4.0;

  if (attempt_control) {
    std::vector<double> input;
    std::vector<int> output;

    input.push_back(uni->pitch());
    input.push_back(uni->roll());
    input.push_back(uni->wheel_velocity());
    input.push_back(uni->yaw_velocity());

    output = nn->feed(input);

    uni->apply_drive_impulse(output[0] * drive_impulse);
    uni->apply_wheel_impulse(output[1] * wheel_impulse);

    std::cout << output[0] * drive_impulse << "\t";
    std::cout << output[1] * wheel_impulse;
  } else {
    std::cout << "0\t0";
  }

  std::cout << "\n";
}

int simulate (Unicycle& user_uni, NN& user_nn, double wheel_imp, double drive_imp)
{
  int argc = 1;
  char* argv[1] = {"maw_simulator"};

  uni = &user_uni;
  nn = &user_nn;

  wheel_impulse = wheel_imp;
  drive_impulse = drive_imp;

  uni->add_to_manager(wm);
  uni->reset();

  app.dynamics_world(wm.dynamics_world());
  app.register_key_handler(handle_key_event);
  app.register_step_callback(simulation_callback);

  std::cout << "# t\tyaw\tpitch\troll\tyaw_velocity\tpitch_velocity\troll_velocity\twheel_velocity\tT\tV\tdrive_imp\twheel_imp\n";

  return glutMain(argc, argv, 800, 600, "Missing A Wheel", &app);
}
