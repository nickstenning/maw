#include <iostream>

#include "MawWorld.h"

int main (int argc, char** argv) {

  MawWorld* mawWorld = new MawWorld();
    
  mawWorld->initPhysics();
  
  for (;;) {
    mawWorld->stepSimulation();
  }
      
  delete mawWorld;
  
  return 0;
}
