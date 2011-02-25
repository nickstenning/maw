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

#include <vector>

#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

class btCollisionShape;
class btDynamicsWorld;
class btRigidBody;
class btTypedConstraint;

class GLShapeDrawer;
class GLDebugDrawer;

class DemoApplication
{
public:

  typedef void (*keyHandler)(unsigned char, int, int);
  typedef void (*callback)();

  DemoApplication();
  virtual ~DemoApplication();

  btDynamicsWorld* dynamicsWorld() const { return m_dynamicsWorld; }
  DemoApplication& dynamicsWorld(btDynamicsWorld* w);

  bool idle() { return m_idle; }
  DemoApplication& idle(bool i) { m_idle = i; return *this; }

  virtual void init();
  virtual void updateCamera();

  btScalar getDeltaTimeMicroseconds() { return btScalar(20000.); }

  void moveAndDisplay();
  virtual void clientMoveAndDisplay() = 0;

  btVector3 getRayTo(int x, int y);

  virtual void updateModifierKeys() = 0;

  virtual void keyboardCallback(unsigned char, int, int);
  virtual void keyboardUpCallback(unsigned char, int, int) {}

  virtual void specialKeyboard(int, int, int) {}
  virtual void specialKeyboardUp(int, int, int) {}

  virtual void reshape(int w, int h);

  virtual void mouseFunc(int button, int state, int x, int y);
  virtual void mouseMotionFunc(int x, int y);

  virtual void displayCallback() {};
  virtual void render();

  void moveCamera(float deltaAzi = 0, float deltaEle = 0);
  void stepLeft();
  void stepRight();
  void stepFront();
  void stepBack();
  void zoomIn();
  void zoomOut();

  void registerKeyHandler(keyHandler handler);
  void registerStepCallback(callback);

protected:
  btDynamicsWorld* m_dynamicsWorld;
  btTypedConstraint* m_pickConstraint; //< constraint for mouse picking

  int m_debugMode;
  bool m_idle;

  float m_ele;
  float m_azi;
  float m_cameraDistance;
  btVector3 m_cameraPosition;
  btVector3 m_cameraTargetPosition;
  btVector3 m_cameraUp;
  int m_forwardAxis;

  btVector3 m_sundirection;
  bool      m_enableshadows;

  int m_mouseOldX;
  int m_mouseOldY;
  int m_mouseButtons;
  int m_modifierKeys;

  float m_scaleBottom;
  float m_scaleFactor;

  int m_screenWidth;
  int m_screenHeight;

  void renderscene(int pass);
  void toggleDebugFlag(int flag);
  void addPickConstraint(btRigidBody* body, btVector3 pickPos);

  GLShapeDrawer* m_shapeDrawer;
  GLDebugDrawer* m_debugDrawer;

  std::vector<keyHandler> m_keyHandlers;
  std::vector<callback> m_stepCallbacks;
private:
  // uncopyable
  DemoApplication(DemoApplication const&);
  DemoApplication& operator=(DemoApplication const&);
};

#endif //DEMO_APPLICATION_H


