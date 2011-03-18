%module nn

%include "typemaps.i"
%include "std_vector.i"

%{
#define SWIG_FILE_WITH_INIT
#include "nn.h"
#include "util.h"
%}

namespace std {
  %template(layersizes_t) vector<size_t>;

  %template(vector_double) vector<double>;
  %template(vector_int) vector<int>;

  %template(matrix_t) vector<vector<double> >;
  %template(weights_t) vector<vector<vector<double> > >;
}

%copyctor NN;
%ignore operator<<;
%ignore operator>>;
%include "nn.h"

%pythoncode %{

NN.__swig_getmethods__["weights"] = NN.weights
if _newclass: NN.weights = _swig_property(NN.weights)

NN.__swig_getmethods__["layers"] = NN.layers
if _newclass: NN.layers = _swig_property(NN.layers)

%}
