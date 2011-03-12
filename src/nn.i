%module nn

%include "std_vector.i"

%{
#define SWIG_FILE_WITH_INIT
#include "nn.h"
#include "util.h"
%}

namespace std {
  %template(layer_spec) vector<size_t>;
  %template(vector_double) vector<double>;
  %template(vector_int) vector<int>;
}

%ignore operator<<;
%ignore operator>>;
%include "nn.h"
