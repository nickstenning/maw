%module nn

%include "typemaps.i"
%include "std_vector.i"

%{
#define SWIG_FILE_WITH_INIT
#include "nn.h"
%}

namespace std {
  %template(layersizes_t) vector<size_t>;

  %template(vector_double) vector<double>;
  %template(vector_int) vector<int>;

  %template(matrix_t) vector<vector<double> >;
  %template(weights_t) vector<vector<vector<double> > >;
}

%copyctor NN;

%rename(_weights) NN::weights;
%rename(_layers) NN::layers;

%include "nn.h"
