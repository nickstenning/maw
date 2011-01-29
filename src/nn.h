#ifndef NN_H
#define NN_H

#include <vector>

class NN
{
public:
  typedef std::vector< double > Layer;
  typedef std::vector< std::vector<double> > WeightMatrix;

  NN();
  NN(size_t numInput, size_t numHidden, size_t numOutput);
  virtual ~NN() {};

  void setRandomWeights();
  bool topologyIsCompatibleWith(NN const& rhs) const;

  std::vector<int> feedForward(std::vector<double> const& input);

  // TODO: remove this friendship and provide accessors for the things Brain
  // wants to modify.
  friend class Brain;

  friend class NNDotPrinter;

  friend std::ostream& operator<< (std::ostream&, NN const&);
  friend std::istream& operator>> (std::istream&, NN&);
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
};

#endif // NN_H
