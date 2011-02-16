/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef DEMO_APPLICATION_H
#define DEMO_APPLICATION_H

#include "GlutStuff.h"
#include "GL_ShapeDrawer.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <LinearMath/btVector3.h>
#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btAlignedObjectArray.h>

class btCollisionShape;
class btDynamicsWorld;
class btRigidBody;
class btTypedConstraint;

class DemoApplication
{

private:
  // uncopyable
  DemoApplication(DemoApplication const&);
  DemoApplication& operator=(DemoApplication const&);

protected:
  btDynamicsWorld* m_dynamicsWorld;

  // constraint for mouse picking
  btTypedConstraint* m_pickConstraint;

  float m_cameraDistance;
  int m_debugMode;

  float m_ele;
  float m_azi;
  btVector3 m_cameraPosition;
  btVector3 m_cameraTargetPosition;

  int m_mouseOldX;
  int m_mouseOldY;
  int m_mouseButtons;
  int m_modifierKeys;

  float m_scaleBottom;
  float m_scaleFactor;
  btVector3 m_cameraUp;
  int m_forwardAxis;

  int m_glutScreenWidth;
  int m_glutScreenHeight;

  bool m_ortho;
  bool m_stepping;
  bool m_singleStep;
  bool m_idle;

  void renderscene(int pass);
  void toggleDebugFlag(int flag);
  void addPickConstraint(btRigidBody* body, btVector3 pickPos);

  GL_ShapeDrawer* m_shapeDrawer;
  bool      m_enableshadows;
  btVector3 m_sundirection;
  btScalar  m_defaultContactProcessingThreshold;

public:

  DemoApplication();
  virtual ~DemoApplication();

  btDynamicsWorld* getDynamicsWorld() {
    return m_dynamicsWorld;
  }

  virtual void initPhysics() = 0;

  void setOrthographicProjection();
  void resetPerspectiveProjection();

  virtual void myinit();
  virtual void updateCamera();

  btScalar getDeltaTimeMicroseconds() {
    return btScalar(16666.);
  }

  void moveAndDisplay();

  virtual void clientMoveAndDisplay() = 0;
  virtual void clientResetScene();

  btVector3 getRayTo(int x, int y);

  btRigidBody* localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape);

  // callback methods from glut

  virtual void updateModifierKeys() = 0;

  virtual void keyboardCallback(unsigned char, int, int);
  virtual void keyboardUpCallback(unsigned char, int, int) {}

  virtual void specialKeyboard(int, int, int) {}
  virtual void specialKeyboardUp(int, int, int) {}

  virtual void reshape(int w, int h);

  virtual void mouseFunc(int button, int state, int x, int y);
  virtual void mouseMotionFunc(int x, int y);

  virtual void displayCallback() {};
  virtual void renderme();

  virtual void swapBuffers() = 0;

  void moveCamera(float deltaAzi = 0, float deltaEle = 0);
  void stepLeft();
  void stepRight();
  void stepFront();
  void stepBack();
  void zoomIn();
  void zoomOut();

  bool isIdle() const {
    return m_idle;
  }
  void setIdle(bool idle) {
    m_idle = idle;
  }
};

#endif //DEMO_APPLICATION_H


