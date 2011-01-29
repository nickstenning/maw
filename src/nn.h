#ifndef NN_H
#define NN_H

#include <istream>
#include <vector>

class NN
{
public:
  typedef std::vector<double> layer;
  typedef layer::iterator layer_i;
  typedef layer::const_iterator layer_ci;

  typedef std::vector<layer> layers;
  typedef layers::iterator layers_i;
  typedef layers::const_iterator layers_ci;

  typedef std::vector<double> weight_vector;
  typedef weight_vector::iterator weight_vector_i;
  typedef weight_vector::const_iterator weight_vector_ci;

  typedef std::vector<weight_vector> weight_matrix;
  typedef weight_matrix::iterator weight_matrix_i;
  typedef weight_matrix::const_iterator weight_matrix_ci;

  typedef std::vector<weight_matrix> weights;

  NN();
  NN(std::vector<size_t> layerSizes);
  NN(std::istream& is);

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
  void initLayers(std::vector<size_t> layerSizes);
  void initWeights();

  void setRandomWeightsForMatrix (size_t index);
  void feedForwardLayer (size_t index);


  inline double activationFunction(double x);
  inline int    terminationFunction(double x);

private:
  layers m_layers;
  weights m_weights;
};

#endif // NN_H
