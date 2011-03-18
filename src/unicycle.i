%module unicycle

%{
#define SWIG_FILE_WITH_INIT

#include <stdexcept>
#include <iostream>
#include <cmath>

#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"
#include "util.h"
%}

%apply double { btScalar };
%include "unicycle.h"
