#ifndef NN_H
#define NN_H

#include <vector>

class NNInputSizeError {};

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
  NN(std::vector<size_t> layer_sizes);

  std::vector<int> feed(std::vector<double> const& input) throw(NNInputSizeError);

  layers_t const& layers() const { return m_layers; }
  layer_t const& layers(size_t i) const { return m_layers[i]; }
  double const& layers(size_t i, size_t j) const { return m_layers[i][j]; }
  NN& layers(size_t i, size_t j, double val) { m_layers[i][j] = val; return *this; }

  weights_t const& weights() const { return m_weights; }
  weight_matrix_t const& weights(size_t k) const { return m_weights[k]; }
  weight_vector_t const& weights(size_t k, size_t i) const { return m_weights[k][i]; }
  double const& weights(size_t k, size_t i, size_t j) const { return m_weights[k][i][j]; }
  NN& weights(size_t k, size_t i, size_t j, double val) { m_weights[k][i][j] = val; return *this; }

protected:
  void feed_layer (size_t index);

  void init_layers(std::vector<size_t> layer_sizes);
  void init_weights();

  inline double activation_function(double x);
  inline int    termination_function(double x);
private:
  layers_t m_layers;
  weights_t m_weights;
};

#endif // NN_H
