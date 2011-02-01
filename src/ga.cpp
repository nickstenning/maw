#include <vector>
#include <algorithm> // std::sort
#include <numeric>   // std::accumulate
#include <cmath>     // round
#include <stdexcept> // std::runtime_error

#include "evolvable.h"
#include "fitness_function.h"
#include "util.h"
#include "ga.h"

namespace GA {

  Evolvable* roulettePick(Population& pop) {
    double wheelSize = std::accumulate(pop.begin(), pop.end(), 0.0, util::pointer_accumulate<Evolvable>());
    double subTotal = 0.0;
    double pickPoint = util::rand(0, wheelSize);

    Population::iterator pick;

    // Find the index that that point in the pie corresponds to.
    for (Population::iterator it = pop.begin(); it != pop.end(); ++it) {
      subTotal += (*it)->fitness();
      if (subTotal >= pickPoint) {
        pick = it;
        break;
      }
    }

    return *pick;
  }

  Runner::Runner (Evolvable* prototype, FitnessFunction* fitnessFunction, size_t numGenerations, size_t popSize)
  : m_pop(popSize)
  , m_prototype(prototype)
  , m_fitnessFunc(fitnessFunction)
  , m_numGenerations(numGenerations)
  , m_generation(0)
  , m_elitism(0.2)
  , m_crossoverProb(0.2)
  , m_mutationProb(1.0)
  {
    size_t numElite = static_cast<size_t>( round(m_elitism * m_pop.size()) );

    for (size_t i = 0; i < m_pop.size(); i += 1) {
      m_pop[i] = m_prototype->clone();

      // Only do random init for non-elite members. This allows us to feed a
      // good network into evolve and seed some proportion (m_elitism) of the
      // population with this network.
      if (i > numElite) m_pop[i]->gaInit();
    }
  }

  Runner::~Runner () {
    for (Population::const_iterator it = m_pop.begin(); it != m_pop.end(); ++it) {
      delete *it;
    }
    m_pop.clear();

    delete m_prototype;
    delete m_fitnessFunc;
  }

  Runner& Runner::elitism(float p) {
    m_elitism = p;
    return *this;
  }

  Runner& Runner::crossoverProb(float p) {
    m_crossoverProb = p;
    return *this;
  }

  Runner& Runner::mutationProb(float p) {
    m_mutationProb = p;
    return *this;
  }

  bool Runner::isFinished () const {
    return m_generation >= m_numGenerations;
  }

  Runner& Runner::step () {
    Population newPop;

    m_generation += 1;

    // Elitist step. We preserve some proportion of the population untouched
    // for the next generation.
    size_t numElite = static_cast<size_t>( round(m_elitism * m_pop.size()) );

    for (Population::iterator it = m_pop.end() - 1; it != m_pop.end() - 1 - numElite; --it) {
      newPop.push_back(m_prototype->clone(*it));
    }

    // Socially liberal step.
    size_t numRemain = m_pop.size() - numElite;

    for (size_t i = 0; i < numRemain; i += 1) {
      Evolvable* obj = m_prototype->clone(roulettePick(m_pop));

      if (util::choose(m_crossoverProb)) {
        obj->crossover(roulettePick(m_pop));
      }

      if (util::choose(m_mutationProb)) {
        obj->mutate();
      }

      newPop.push_back(obj);
    }

    // Replace population with new population.
    for (Population::const_iterator it = m_pop.begin(); it != m_pop.end(); ++it) {
      delete *it;
    }
    m_pop.clear();
    m_pop = newPop;

    // Compute fitness for next generation
    for(Population::iterator it = m_pop.begin(); it != m_pop.end(); ++it) {
      (*it)->fitness((*m_fitnessFunc)(*it));
    }

    // Generate rank-ordered list by fitness, fittest last.
    sortPopulation();

    return *this;
  }

  void Runner::fillStats (int& gen, double& min, double& max, double& mean, double& stddev) const {
    gen = m_generation;
    min = m_pop[0]->fitness();
    max = m_pop[m_pop.size() - 1]->fitness();
    mean = std::accumulate(m_pop.begin(), m_pop.end(), 0.0, util::pointer_accumulate<Evolvable>()) / m_pop.size();

    double sumsqdev = 0.0;

    for (Population::const_iterator it = m_pop.begin(); it != m_pop.end(); ++it) {
      sumsqdev += pow((*it)->fitness() - mean, 2);
    }

    stddev = sqrt(sumsqdev / m_pop.size());
  }

  Evolvable const* Runner::getFittest () {
    sortPopulation();
    return m_pop[m_pop.size() - 1];
  }

  void Runner::sortPopulation() {
    std::sort(m_pop.begin(), m_pop.end(), util::pointer_compare<Evolvable>());
  }
}


