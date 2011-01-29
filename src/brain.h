#ifndef BRAIN_H
#define BRAIN_H

#include <vector>

#include "evolvable.h"

#define MUTATION_SIZE 2.0

class BrainDotPrinter;

class Brain : public Evolvable
{
public:
  typedef std::vector< double > Layer;
  typedef std::vector< std::vector<double> > WeightMatrix;

  Brain();
  Brain(size_t numInput, size_t numHidden, size_t numOutput);

  virtual void gaInit();
  virtual Evolvable* clone ();
  virtual Evolvable* clone (Evolvable* copy);
  virtual Evolvable* mutate ();
  virtual Evolvable* crossover (Evolvable const* other);

  void setRandomWeights();

  bool topologyIsCompatibleWith(Brain const& rhs) const;

  std::vector<int> feedForward(std::vector<double> const& input);

  friend std::ostream& operator<<(std::ostream&, Brain const&);
  friend std::istream& operator>> (std::istream& is, Brain& b);

  friend class BrainDotPrinter;

protected:
  void initLayers();
  void initWeights();

  inline double activationFunction(double x);
  inline int    terminationFunction(double x);

private:
  size_t m_numInput;
  size_t m_numHidden;
  size_t m_numOutput;

  Layer m_layerInput;
  Layer m_layerHidden;
  Layer m_layerOutput;

  WeightMatrix m_weightsIH;
  WeightMatrix m_weightsHO;

  double m_mutationRate;
  double m_mutationSize;
};

// Define operator for template in "util.h"
std::ostream& operator<<(std::ostream&, std::vector<Brain> const&);

#endif // BRAIN_H