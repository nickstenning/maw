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
  , m_target(0.0)
{}

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

  m_uni.reset();

  m_uni.pitch(util::rand(-SCORE_ANG, SCORE_ANG))
  m_uni.roll(util::rand(-SCORE_ANG, SCORE_ANG))

  while (m_uni.t() < MAX_EVAL_TIME) {
    m_target = 0.0;

    if (m_uni.t() > MAX_EVAL_TIME / 3.0) {
      m_target = 2.0;
    } else if (m_uni.t() > (2.0 * MAX_EVAL_TIME) / 3.0) {
      m_target = -2.0;
    }

    step(brain);

    bool inScoringZone = std::abs(m_uni.p()) < SCORE_ANG;

    if (inScoringZone) {
      double posScore = util::diracDelta(m_uni.p(), 5);
      double movScore = util::diracDelta(m_uni.dwdt() - m_target, 5);
      fitness += m_uni.dt * (0.5 * posScore + 0.5 * movScore);
    } else {
      break; // Failure. No need to evaluate further.
    }
  }

  return fitness;
}

void Unicycle2DFitnessFunction::step (Brain* brain)
{
  std::vector<double> input;
  std::vector<int> output;

  // We don't provide w as symmetry implies it can't be useful!
  input.push_back(m_uni.p());
  input.push_back(m_uni.dpdt());
  input.push_back(m_uni.dwdt());
  input.push_back(m_target);

  output = brain->feedForward(input);

  double controlForce = BANG_SIZE * output[0];

  m_uni.step(m_uni.dt, controlForce + util::rand(-NOISE_SIZE, NOISE_SIZE));
}
