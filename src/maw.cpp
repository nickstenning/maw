#include <iostream>

#include "maw_world.h"
#include "GlutStuff.h"

int main (int argc, char** argv) {
  MawWorld mawWorld;
  mawWorld.initPhysics();

	return glutmain(argc, argv, 900, 600, "Bullet MAW demo", &mawWorld);
}
