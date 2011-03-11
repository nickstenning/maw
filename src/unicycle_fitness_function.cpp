#include <cmath> // double std::abs(double)
#include <stdexcept>

#include "evolvable.h"
#include "brain.h"
#include "unicycle.h"
#include "world_manager.h"
#include "util.h"
#include "unicycle_fitness_function.h"

UnicycleFitnessFunction::UnicycleFitnessFunction()
  : m_uni()
  , m_world()
  , m_time(0.0)
  , m_dt(0.02)
{
  m_uni.addToManager(m_world);
}

/**
 * Feed the state variables into the brain, feed forward, and advance the
 * physics using the output of the brain as a controlling torque.
**/
double UnicycleFitnessFunction::operator() (Evolvable* obj)
{
  Brain* brain = dynamic_cast<Brain*>(obj);

  if (brain == NULL) {
    throw std::runtime_error("UnicycleFitnessFunction received an Evolvable instance that wasn't a Brain!");
  }

  double fitness = 0.0;

  m_time = 0.0;
  m_uni.reset();
  m_uni.computeState();

  while (m_time < MAX_EVAL_TIME) {
    step(brain);

    bool inScoringZone = std::abs(m_uni.yaw()) < YAW_SCORE_ANG &&
                         std::abs(m_uni.pitch()) < PITCH_SCORE_ANG &&
                         std::abs(m_uni.roll()) < ROLL_SCORE_ANG;

    if (inScoringZone) {
      double score = util::diracDelta(m_uni.pitch(), 5) + util::diracDelta(m_uni.roll(), 5);
      fitness += m_dt * score;
    } else {
      break; // Failure. No need to evaluate further.
    }
  }

  return fitness;
}

void UnicycleFitnessFunction::step (Brain* brain)
{
  std::vector<double> input;
  std::vector<int> output;

  input.push_back(m_uni.yaw());
  input.push_back(m_uni.pitch());
  input.push_back(m_uni.roll());
  input.push_back(m_uni.wheelVelocity());

  output = brain->feedForward(input);

  double yawImpulse = YAW_BANG_SIZE * output[0];
  double pitchImpulse = PITCH_BANG_SIZE * output[1];

  m_uni.applyForkImpulse(yawImpulse);
  m_uni.applyWheelImpulse(pitchImpulse);

  m_world.stepSimulation(m_dt);
  m_uni.computeState();

  m_time += m_dt;
}
