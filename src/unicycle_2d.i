%module unicycle_2d

%{
#define SWIG_FILE_WITH_INIT
#include <cmath>
#include <algorithm>

#include "util.h"
#include "vector.h"
#include "unicycle_2d.h"
%}

%include "unicycle_2d.h"

%pythoncode %{

Unicycle2D.__swig_getmethods__["p"] = Unicycle2D.p
Unicycle2D.__swig_setmethods__["p"] = Unicycle2D.p
if _newclass: Unicycle2D.p = _swig_property(Unicycle2D.p, Unicycle2D.p)  

Unicycle2D.__swig_getmethods__["dpdt"] = Unicycle2D.dpdt
Unicycle2D.__swig_setmethods__["dpdt"] = Unicycle2D.dpdt
if _newclass: Unicycle2D.dpdt = _swig_property(Unicycle2D.dpdt, Unicycle2D.dpdt)

Unicycle2D.__swig_getmethods__["w"] = Unicycle2D.w
Unicycle2D.__swig_setmethods__["w"] = Unicycle2D.w
if _newclass: Unicycle2D.w = _swig_property(Unicycle2D.w, Unicycle2D.w)

Unicycle2D.__swig_getmethods__["dwdt"] = Unicycle2D.dwdt
Unicycle2D.__swig_setmethods__["dwdt"] = Unicycle2D.dwdt
if _newclass: Unicycle2D.dwdt = _swig_property(Unicycle2D.dwdt, Unicycle2D.dwdt)

Unicycle2D.__swig_getmethods__["t"] = Unicycle2D.t
if _newclass: Unicycle2D.t = _swig_property(Unicycle2D.t)

Unicycle2D.__swig_getmethods__["T"] = Unicycle2D.T
if _newclass: Unicycle2D.T = _swig_property(Unicycle2D.T)

Unicycle2D.__swig_getmethods__["V"] = Unicycle2D.V
if _newclass: Unicycle2D.V = _swig_property(Unicycle2D.V)

%}
