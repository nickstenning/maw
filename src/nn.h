#ifndef NN_H
#define NN_H

#include <iostream>
#include <vector>

class NN
{
public:
  typedef std::vector<double> layer_t;
  typedef std::vector<layer_t> layers_t;

  typedef std::vector<double> weight_vector_t;
  typedef std::vector<weight_vector_t> weight_matrix_t;
  typedef std::vector<weight_matrix_t> weights_t;

  typedef layer_t input_t;
  typedef std::vector<int> output_t;

  NN();
  NN(std::vector<size_t> layerSizes);
  NN(std::istream& is);

  virtual ~NN() {};

  void setRandomWeights();
  bool topologyIsCompatibleWith(NN const& rhs) const;

  std::vector<int> feedForward(std::vector<double> const& input);

  layers_t const& layers() const {
    return m_layers;
  }
  weights_t const& weights() const {
    return m_weights;
  }

  // TODO: remove this friendship and provide accessors for the things Brain
  // wants to modify.
  friend class Brain;

  friend class NNDotPrinter;

  friend std::ostream& operator<< (std::ostream&, NN const&);
  friend std::istream& operator>> (std::istream&, NN&);
protected:
  void initLayers(std::vector<size_t> layerSizes);
  void initWeights();

  void setRandomWeightsForMatrix (size_t index);
  void feedForwardLayer (size_t index);

  inline double activationFunction(double x);
  inline int    terminationFunction(double x);
private:
  layers_t m_layers;
  weights_t m_weights;
};

#endif // NN_H
