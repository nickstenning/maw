#ifndef MAW_SIMULATOR_H
#define MAW_SIMULATOR_H

#include <LinearMath/btScalar.h>

class NN;
class Unicycle;

int simulate(Unicycle&, NN&, btScalar yaw_bang_size, btScalar pitch_bang_size);

#endif // MAW_SIMULATOR_H
