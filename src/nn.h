#ifndef NN_H
#define NN_H

#include <iostream>
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
  NN(std::istream& is);

  void set_weights_random();
  bool topology_is_compatible(NN const& rhs) const;

  std::vector<int> feed(std::vector<double> const& input) throw(NNInputSizeError);

  layers_t const& layers() const {
    return m_layers;
  }
  weights_t const& weights() const {
    return m_weights;
  }

  void set_weight(size_t layer, size_t from, size_t to, double value) {
    m_weights[layer][from][to] = value;
  }

  friend std::istream& operator>> (std::istream&, NN&);

protected:
  void init_layers(std::vector<size_t> layer_sizes);
  void init_weights();

  void feed_layer (size_t index);

  inline double activation_function(double x);
  inline int    termination_function(double x);
private:
  layers_t m_layers;
  weights_t m_weights;
};

#endif // NN_H
