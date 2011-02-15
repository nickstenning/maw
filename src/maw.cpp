#include "vendor/BulletGL/GlutStuff.h"
#include "maw_world.h"

int main (int argc, char** argv) {
  MawWorld mawWorld;
  mawWorld.initPhysics();

  return glutmain(argc, argv, 900, 600, "Bullet MAW demo", &mawWorld);
}
