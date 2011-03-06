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

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h> // picking

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btUniformScalingShape.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>

#include "OpenGL.h"
#include "GLShapeDrawer.h"
#include "GLDebugDrawer.h"

#include "DemoApplication.h"

extern bool gDisableDeactivation;
int numObjects = 0;
const int maxNumObjects = 16384;
btTransform startTransforms[maxNumObjects];
btCollisionShape* gShapePtr[maxNumObjects]; // 1 rigidbody has 1 shape (no re-use of shapes)

extern int gNumClampedCcdMotions;

void toggle(bool& flag)
{
  flag = !flag;
}

DemoApplication::DemoApplication()
  : m_dynamicsWorld(0)
  , m_pickConstraint(0)
  , m_debugMode(0)
  , m_idle(false)
  , m_ele(30)
  , m_azi(0)
  , m_cameraDistance(15)
  , m_cameraPosition(0, 0, 0)
  , m_cameraTargetPosition(0, 0, 0)
  , m_cameraUp(0, 1, 0)
  , m_forwardAxis(2)
  , m_sundirection(btVector3(1, -2, 1) * 1000) // direction of travel of *rays*
  , m_enableshadows(false)
  , m_mouseOldX(0)
  , m_mouseOldY(0)
  , m_mouseButtons(0)
  , m_modifierKeys(0)
  , m_scaleBottom(0.5)
  , m_scaleFactor(2.0)
  , m_screenWidth(0)
  , m_screenHeight(0)
  , m_shapeDrawer(new GLShapeDrawer())
  , m_debugDrawer(new GLDebugDrawer())
  , m_keyHandlers()
  , m_stepCallbacks()
{
  m_shapeDrawer->enableTexture(true);
}

DemoApplication::~DemoApplication()
{
  if (m_shapeDrawer) { delete m_shapeDrawer; }
  if (m_debugDrawer) { delete m_debugDrawer; }
}

DemoApplication& DemoApplication::dynamicsWorld(btDynamicsWorld* w)
{
  m_dynamicsWorld = w;
  m_dynamicsWorld->setDebugDrawer(m_debugDrawer);
  return *this;
}


void DemoApplication::init(void)
{
  GLfloat light_ambient[]  = { btScalar(0.2), btScalar(0.2), btScalar(0.2), btScalar(1.0) };
  GLfloat light_diffuse[]  = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0) };
  GLfloat light_specular[] = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0 )};

  /* light_position is NOT default value */
  GLfloat light_position0[] = { btScalar(1.0), btScalar(10.0), btScalar(1.0), btScalar(0.0 )};
  GLfloat light_position1[] = { btScalar(-1.0), btScalar(-10.0), btScalar(-1.0), btScalar(0.0) };

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glClearColor(btScalar(0.5), btScalar(0.5), btScalar(0.5), btScalar(0));

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void DemoApplication::updateCamera()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  btScalar rele = m_ele * btScalar(M_PI / 180.0);
  btScalar razi = m_azi * btScalar(M_PI / 180.0);

  btQuaternion rot(m_cameraUp, razi);
  btVector3 eyePos(0, 0, 0);

  eyePos[m_forwardAxis] = - m_cameraDistance;

  btVector3 forward(eyePos[0], eyePos[1], eyePos[2]);

  if (forward.length2() < SIMD_EPSILON) {
    forward.setValue(1, 0, 0);
  }

  btVector3 right = m_cameraUp.cross(forward);
  btQuaternion roll(right, -rele);

  eyePos = btMatrix3x3(rot) * btMatrix3x3(roll) * eyePos;

  m_cameraPosition = eyePos + m_cameraTargetPosition;

  if (m_screenWidth == 0 && m_screenHeight == 0) {
    return;
  }

  btScalar aspect;
  btVector3 extents;

  if (m_screenWidth > m_screenHeight) {
    aspect = m_screenWidth / static_cast<btScalar>(m_screenHeight);
    extents.setValue(aspect * 1.0, 1.0, 0);
  } else {
    aspect = m_screenHeight / static_cast<btScalar>(m_screenWidth);
    extents.setValue(1.0, aspect * 1.0, 0);
  }

  if (m_screenWidth > m_screenHeight) {
    glFrustum(-aspect, aspect, -1.0, 1.0, 1.0, 10000.0);
  } else {
    glFrustum(-1.0, 1.0, -aspect, aspect, 1.0, 10000.0);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(
    m_cameraPosition[0], m_cameraPosition[1], m_cameraPosition[2],
    m_cameraTargetPosition[0], m_cameraTargetPosition[1], m_cameraTargetPosition[2],
    m_cameraUp.getX(), m_cameraUp.getY(), m_cameraUp.getZ()
  );
}

void DemoApplication::moveCamera(float deltaAzi, float deltaEle)
{
  m_azi += deltaAzi;
  m_ele += deltaEle;
  while (m_azi < 0)    { m_azi += 360; }
  while (m_azi >= 360) { m_azi -= 360; }
  while (m_ele < 0)    { m_ele += 360; }
  while (m_ele >= 360) { m_ele -= 360; }
  updateCamera();
}

void DemoApplication::stepLeft()
{
  moveCamera( 5,  0);
}
void DemoApplication::stepRight()
{
  moveCamera(-5,  0);
}
void DemoApplication::stepFront()
{
  moveCamera( 0,  5);
}
void DemoApplication::stepBack()
{
  moveCamera( 0, -5);
}

void DemoApplication::zoomIn()
{
  m_cameraDistance -= btScalar(0.4);
  if (m_cameraDistance < btScalar(0.1)) {
    m_cameraDistance = btScalar(0.1);
  }
  updateCamera();
}

void DemoApplication::zoomOut()
{
  m_cameraDistance += btScalar(0.4);
  updateCamera();
}

void DemoApplication::reshape(int w, int h)
{
  m_screenWidth = w;
  m_screenHeight = h;

  glViewport(0, 0, w, h);
  updateCamera();
}

void DemoApplication::keyboardCallback(unsigned char key, int x, int y)
{
  switch (key) {
    case 'q': exit(0);                 break;

    case '=': zoomIn();                break;
    case '-': zoomOut();               break;
    case 'i': toggle(m_idle);          break;
    case 's': toggle(m_enableshadows); break;

    case 'w': toggleDebugFlag(GLDebugDrawer::DBG_DrawWireframe);        break;
    case 'a': toggleDebugFlag(GLDebugDrawer::DBG_DrawAabb);             break;
    case 'c': toggleDebugFlag(GLDebugDrawer::DBG_DrawContactPoints);    break;
    case 'C': toggleDebugFlag(GLDebugDrawer::DBG_DrawConstraints);      break;
    case 'L': toggleDebugFlag(GLDebugDrawer::DBG_DrawConstraintLimits); break;
    case 'd': toggleDebugFlag(GLDebugDrawer::DBG_NoDeactivation);       break;

    default: break;
  }

  for (std::vector<DemoApplication::keyHandler>::iterator it = m_keyHandlers.begin(); it != m_keyHandlers.end(); ++it) {
    (*it)(key, x, y);
  }
}

void DemoApplication::toggleDebugFlag(int modeFlag)
{
  if (m_debugMode & modeFlag) {
    m_debugMode = m_debugMode & (~modeFlag);
  } else {
    m_debugMode |= modeFlag;
  }

  if (dynamicsWorld() && dynamicsWorld()->getDebugDrawer()) {
    dynamicsWorld()->getDebugDrawer()->setDebugMode(m_debugMode);
  }
}

void DemoApplication::moveAndDisplay()
{
  if (!m_idle) {
    clientMoveAndDisplay();

    for (std::vector<DemoApplication::callback>::iterator it = m_stepCallbacks.begin(); it != m_stepCallbacks.end(); ++it) {
      (*it)();
    }
  } else {
    displayCallback();
  }
}

int gPickingConstraintId = 0;
btVector3 gOldPickingPos;
btVector3 gHitPos(-1, -1, -1);
float gOldPickingDist  = 0.0;
btRigidBody* pickedBody = 0; // for deactivation state
int pickedBodyActivationState = 0;

btVector3 DemoApplication::getRayTo(int x, int y)
{
  float top = 1.0;
  float bottom = -1.0;
  float nearPlane = 1.0;
  float tanFov = (top - bottom) * 0.5 / nearPlane;
  float fov = btScalar(2.0) * btAtan(tanFov);

  btVector3 rayFrom = m_cameraPosition;
  btVector3 rayForward = m_cameraTargetPosition - rayFrom;
  rayForward.normalize();
  float farPlane = 10000.0;
  rayForward *= farPlane;

  btVector3 rightOffset;
  btVector3 vertical = m_cameraUp;

  btVector3 hor;
  hor = rayForward.cross(vertical);
  hor.normalize();
  vertical = hor.cross(rayForward);
  vertical.normalize();

  float tanfov = tanf(0.5 * fov);


  hor *= 2.0 * farPlane * tanfov;
  vertical *= 2.0 * farPlane * tanfov;

  btScalar aspect;

  if (m_screenWidth > m_screenHeight) {
    aspect = m_screenWidth / (btScalar)m_screenHeight;

    hor *= aspect;
  } else {
    aspect = m_screenHeight / (btScalar)m_screenWidth;
    vertical *= aspect;
  }

  btVector3 rayToCenter = rayFrom + rayForward;
  btVector3 dHor = hor * 1.0 / float(m_screenWidth);
  btVector3 dVert = vertical * 1.0 / float(m_screenHeight);

  btVector3 rayTo = rayToCenter - 0.5 * hor + 0.5 * vertical;
  rayTo += btScalar(x) * dHor;
  rayTo -= btScalar(y) * dVert;
  return rayTo;
}

void DemoApplication::mouseFunc(int button, int state, int x, int y)
{
  if (!m_dynamicsWorld) { return; }

  if (state == 0) {
    m_mouseButtons |= 1 << button;
  } else {
    m_mouseButtons = 0;
  }

  m_mouseOldX = x;
  m_mouseOldY = y;

  updateModifierKeys();

  btVector3 rayTo = getRayTo(x, y);

  switch (button) {
    case 2:
      break; // Right-click
    case 1:
      break; // Middle-click

    case 0: { // Left-click: add a point to point constraint for picking
      if (state == 0) {
        btVector3 rayFrom;

        rayFrom = m_cameraPosition;

        btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
        m_dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

        if (rayCallback.hasHit()) {
          btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
          btVector3 pickPos = rayCallback.m_hitPointWorld;

          addPickConstraint(body, pickPos);

          //save mouse position for dragging
          gOldPickingPos = rayTo;
          gHitPos = pickPos;
          gOldPickingDist = (pickPos - rayFrom).length();
        }
      } else { // On click release, release pick constraint
        if (m_pickConstraint) {
          m_dynamicsWorld->removeConstraint(m_pickConstraint);
          delete m_pickConstraint;
          m_pickConstraint = 0;

          pickedBody->forceActivationState(pickedBodyActivationState);
          pickedBody = 0;
        }
      }

      break;

    }
    default:
      break;
  }

}

void DemoApplication::addPickConstraint(btRigidBody* body, btVector3 pickPos)
{
  if (!m_dynamicsWorld || !body || body->isStaticObject()) {
    return;
  }

  pickedBody = body;
  pickedBodyActivationState = pickedBody->getActivationState();
  pickedBody->setActivationState(DISABLE_DEACTIVATION);

  btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;

  btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
  m_dynamicsWorld->addConstraint(p2p);
  m_pickConstraint = p2p;
  p2p->m_setting.m_impulseClamp = 30.0;
  p2p->m_setting.m_tau = 0.1; // very weak constraint for picking
}

void DemoApplication::mouseMotionFunc(int x, int y)
{
  if (m_pickConstraint) {
    // move the constraint pivot
    btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickConstraint);

    if (pickCon) {
      // keep it at the same picking distance
      btVector3 newRayTo = getRayTo(x, y);
      btVector3 rayFrom;
      btVector3 oldPivotInB = pickCon->getPivotInB();
      btVector3 newPivotB;

      rayFrom = m_cameraPosition;
      btVector3 dir = newRayTo - rayFrom;
      dir.normalize();
      dir *= gOldPickingDist;
      newPivotB = rayFrom + dir;

      pickCon->setPivotB(newPivotB);
    }
  }

  float dx, dy;

  dx = btScalar(x) - m_mouseOldX;
  dy = btScalar(y) - m_mouseOldY;

  m_mouseOldX = x;
  m_mouseOldY = y;

  updateCamera();
}

void DemoApplication::renderscene(int pass)
{
  btScalar m[16];
  btMatrix3x3 rot;
  rot.setIdentity();

  const int numObj = m_dynamicsWorld->getNumCollisionObjects();
  btVector3 wireColor(1, 0, 0);

  for (int i = 0; i < numObj; i++) {

    btCollisionObject* colObj = m_dynamicsWorld->getCollisionObjectArray()[i];

    colObj->getWorldTransform().getOpenGLMatrix(m);
    rot = colObj->getWorldTransform().getBasis();

    wireColor = btVector3(1.0, 1.0, 0.5); // wants deactivation

    if (i & 1) {
      wireColor = btVector3(0.0, 0.0, 1.0);
    }

    // color differently for active, sleeping, wantsdeactivation states
    if (colObj->getActivationState() == 1) { //active
      if (i & 1) {
        wireColor += btVector3 (1.0, 0.0, 0.0);
      } else {
        wireColor += btVector3 (.5f, 0.0, 0.0);
      }
    }

    if (colObj->getActivationState() == 2) { // ISLAND_SLEEPING
      if (i & 1) {
        wireColor += btVector3(0.0, 1.0, 0.0);
      } else {
        wireColor += btVector3(0.0, 0.5, 0.0);
      }
    }

    btVector3 aabbMin, aabbMax;
    m_dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin, aabbMax);

    aabbMin -= btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
    aabbMax += btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);

    if (!(m_debugMode & GLDebugDrawer::DBG_DrawWireframe)) {
      switch (pass) {
        case 0:
          m_shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, m_debugMode, aabbMin, aabbMax);
          break;
        case 1:
          m_shapeDrawer->drawShadow(m, m_sundirection * rot, colObj->getCollisionShape(), aabbMin, aabbMax);
          break;
        case 2:
          m_shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor * btScalar(0.3), 0, aabbMin, aabbMax);
          break;
      }
    }
  }
}

void DemoApplication::render()
{
  init();
  updateCamera();

  if (m_dynamicsWorld) {
    if (m_enableshadows) {
      glClear(GL_STENCIL_BUFFER_BIT);
      glEnable(GL_CULL_FACE);

      renderscene(0);

      glDisable(GL_LIGHTING);
      glDepthMask(GL_FALSE);
      glDepthFunc(GL_LEQUAL);
      glEnable(GL_STENCIL_TEST);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFFL);
      glFrontFace(GL_CCW);
      glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

      renderscene(1);

      glFrontFace(GL_CW);
      glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

      renderscene(1);

      glFrontFace(GL_CCW);
      glPolygonMode(GL_FRONT, GL_FILL);
      glPolygonMode(GL_BACK, GL_FILL);
      glShadeModel(GL_SMOOTH);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glEnable(GL_LIGHTING);
      glDepthMask(GL_TRUE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      glEnable(GL_CULL_FACE);
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthFunc(GL_LEQUAL);
      glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFFL);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
      glDisable(GL_LIGHTING);

      renderscene(2);

      glEnable(GL_LIGHTING);
      glDepthFunc(GL_LESS);
      glDisable(GL_STENCIL_TEST);
      glDisable(GL_CULL_FACE);
    } else {
      renderscene(0);
    }
  }

  updateCamera();

}

void DemoApplication::registerKeyHandler(DemoApplication::keyHandler handler)
{
  if (handler) {
    m_keyHandlers.push_back(handler);
  }
}

void DemoApplication::registerStepCallback(DemoApplication::callback cb)
{
  if (cb) {
    m_stepCallbacks.push_back(cb);
  }
}