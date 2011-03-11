#ifndef UNICYCLE_FITNESS_FUNCTION_H
#define UNICYCLE_FITNESS_FUNCTION_H

#include "fitness_function.h"
#include "unicycle.h"
#include "world_manager.h"

// Unicycle config
#define YAW_BANG_SIZE     2.0
#define PITCH_BANG_SIZE   2.0
#define MAX_EVAL_TIME     100.0
#define PI                3.141592653589793238462643
#define YAW_SCORE_ANG     (9.0 * PI / 10.0)
#define PITCH_SCORE_ANG   (PI / 12.0)
#define ROLL_SCORE_ANG    (PI / 12.0)

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
  Unicycle m_uni;
  WorldManager m_world;

  double m_time;
  double m_dt;
};

#endif // UNICYCLE_FITNESS_FUNCTION_H
