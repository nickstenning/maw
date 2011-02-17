#ifndef GA_H
#define GA_H

#include <iostream>
#include <vector>

class Evolvable;
class FitnessFunction;

namespace GA
{

  /**
   * A wrapper around a population of instances of Evolvable.
  **/
  typedef std::vector<Evolvable*> Population;

  /**
   * Pick members from a population, weighted towards those with higher fitness.
  **/
  Evolvable* roulettePick(Population& pop);

  /**
   * A Runner helps create and perform GA on a Population of Evolvable objects.
  **/
  class Runner
  {
  public:
    Runner (Evolvable* prototype,
            FitnessFunction* fitnessFunction,
            size_t numGenerations,
            size_t popSize);

    ~Runner ();

    Runner& elitism (float p);
    Runner& crossoverProb (float p);
    Runner& mutationProb (float p);

    bool isFinished () const;
    Runner& step ();

    Evolvable const* getFittest ();

    void fillStats (int& gen, double& min, double& max, double& mean, double& stddev) const;

  protected:
    void sortPopulation ();

    // No copying
    Runner(const GA::Runner&);
    Runner& operator=(const GA::Runner&);

  private:
    Population m_pop;

    Evolvable* m_prototype;
    FitnessFunction* m_fitnessFunc;

    size_t m_numGenerations;
    size_t m_generation;

    float m_elitism;
    float m_crossoverProb;
    float m_mutationProb;
  };
}


#endif // GA_H