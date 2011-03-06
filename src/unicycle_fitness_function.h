#ifndef UNICYCLE_FITNESS_FUNCTION_H
#define UNICYCLE_FITNESS_FUNCTION_H

#include "fitness_function.h"
#include "unicycle.h"
#include "world_manager.h"

// Unicycle config
#define BANG_SIZE     20.0
#define NOISE_SIZE    0.0
#define MAX_EVAL_TIME 100.0
#define PI            3.141592653589793238462643
#define SCORE_ANG     (PI / 12.0)

class Evolvable;
class Brain;

class UnicycleFitnessFunction : public FitnessFunction
{
public:
  UnicycleFitnessFunction();
  virtual double operator() (Evolvable* obj);
protected:
  void step (Brain* brain);
private:
  WorldManager m_world;
  Unicycle m_uni;
  double m_target;
};

#endif // UNICYCLE_FITNESS_FUNCTION_H
