#ifndef FITNESS_FUNCTION_H
#define FITNESS_FUNCTION_H

class Evolvable;

/**
 * A fitness function returns a double after evaluating an Evolvable instance.
**/
class FitnessFunction
{
public:
  virtual ~FitnessFunction() {};
  virtual double operator() (Evolvable*) { return 0; };
};

#endif // FITNESS_FUNCTION_H

