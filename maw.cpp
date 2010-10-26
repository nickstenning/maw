#include <iostream>

#include "MawWorld.h"

int main (int argc, char* const argv[]) {

  MawWorld mawWorld;

  mawWorld.initPhysics();

  for (;;) {
    mawWorld.stepSimulation();
  }

  return 0;
}
