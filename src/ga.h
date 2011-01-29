#ifndef GA_H
#define GA_H

#include <iostream>
#include <vector>

class Evolvable;
class FitnessFunction;

namespace GA {

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

/**
 * pointer_comparison allows us to use std::sort on a std::vector<Class*>
**/
template <typename T>
class pointer_comparison
{
public:
  const bool operator() (T const* a, T const* b) const {
    // check for 0
    if (a == 0) {
      return b != 0; // if b is also 0, then they are equal, hence a is not < than b
    } else if (b == 0) {
      return false;
    } else {
      return *a < *b;
    }
  }
};

/**
 * pointer_comparison allows us to use std::accumulate on a std::vector<Class*>
**/
template <typename T>
class pointer_accumulate
{
public:
  const double operator() (double const a, T const* b) const {
    return a + *b;
  }
};


#endif // GA_H