#include <iostream>
#include "GLDebugDrawer.h"
#include "GLDebugFont.h"

#include "MawWorld.h"
#include "GL_ShapeDrawer.h"
#include "GlutStuff.h"

int main (int argc, char** argv) {

  GLDebugDrawer debugDrawer;
  MawWorld mawWorld;

  mawWorld.initPhysics();
  mawWorld.setDebugDrawer(debugDrawer);
  mawWorld.setCameraDistance(15.f);

	return glutmain(argc, argv, 1024, 768, "Bullet MAW demo", &mawWorld);
}
