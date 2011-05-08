#ifndef MAW_SIMULATOR_H
#define MAW_SIMULATOR_H

class NN;
class Unicycle;

int simulate(double dt, Unicycle&, NN&, double wheel_imp, double drive_imp);

#endif // MAW_SIMULATOR_H
