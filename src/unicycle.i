%module unicycle

%{
#define SWIG_FILE_WITH_INIT
#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
#include "unicycle.h"
%}

%apply double { btScalar };
%include "unicycle.h"
