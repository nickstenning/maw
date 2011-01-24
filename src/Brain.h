#ifndef BRAIN_H
#define BRAIN_H

#include <vector>

typedef std::vector< double > Layer;
typedef std::vector< std::vector<double> > Weights;

class BrainDotPrinter;

class Brain {
public:
  Brain(size_t numInput, size_t numHidden, size_t numOutput);
  ~Brain();

  void initRandomWeights();

  double const& fitness() const;
  Brain const& fitness(double const&);

  Weights const& weightsHidden() const;
  Brain& weightsHidden(Weights const&);
  Weights const& weightsOutput() const;
  Brain& weightsOutput(Weights const&);

  bool topologyIsCompatibleWith(Brain const& rhs) const;

  std::vector<int> feedForward(std::vector<double> const& input);

  friend std::ostream& operator<<(std::ostream&, Brain const&);
  friend class BrainDotPrinter;

protected:
  double m_fitness;

  size_t m_numInput;
  size_t m_numHidden;
  size_t m_numOutput;

  Layer m_layerInput;
  Layer m_layerHidden;
  Layer m_layerOutput;

  Weights m_weightsIH;
  Weights m_weightsHO;

  void initLayer(Layer& l, size_t numNeurons);

  inline double activationFunction(double x);
  inline int    terminationFunction(double x);
};

// Define operator for template in "util.h"
std::ostream& operator<<(std::ostream&, std::vector<Brain> const&);

#endif // BRAIN_H