%module world_manager

%{
#define SWIG_FILE_WITH_INIT
#include <BulletDynamics/btBulletDynamicsCommon.h>

#include "world_manager.h"
%}
                   
%apply double { btScalar };
%ignore operator<<;
%include "world_manager.h"
