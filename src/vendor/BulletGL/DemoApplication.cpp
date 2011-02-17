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


#include "DemoApplication.h"

#include <LinearMath/btIDebugDraw.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h> // picking
#include <BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h> // picking

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btUniformScalingShape.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>
#include "GL_ShapeDrawer.h"

#include <LinearMath/btDefaultMotionState.h>
#include <LinearMath/btSerializer.h>

extern bool gDisableDeactivation;
int numObjects = 0;
const int maxNumObjects = 16384;
btTransform startTransforms[maxNumObjects];
btCollisionShape* gShapePtr[maxNumObjects];//1 rigidbody has 1 shape (no re-use of shapes)

extern int gNumClampedCcdMotions;

void toggle(bool& flag)
{
  flag = !flag;
}

// see btIDebugDraw.h for modes
DemoApplication::DemoApplication()
  : m_dynamicsWorld(0)
  , m_pickConstraint(0)
  , m_cameraDistance(15)
  , m_debugMode(0)
  , m_ele(20)
  , m_azi(0)
  , m_cameraPosition(0, 0, 0)
  , m_cameraTargetPosition(0, 0, 0)
  , m_mouseOldX(0)
  , m_mouseOldY(0)
  , m_mouseButtons(0)
  , m_modifierKeys(0)
  , m_scaleBottom(0.5)
  , m_scaleFactor(2.0)
  , m_cameraUp(0, 1, 0)
  , m_forwardAxis(2)
  , m_glutScreenWidth(0)
  , m_glutScreenHeight(0)
  , m_ortho(false)
  , m_stepping(true)
  , m_singleStep(false)
  , m_idle(false)
  , m_shapeDrawer()
  , m_enableshadows(false)
  , m_sundirection(btVector3(1, -2, 1) * 1000)
  , m_defaultContactProcessingThreshold(BT_LARGE_FLOAT)
{
  m_shapeDrawer = new GL_ShapeDrawer ();
  m_shapeDrawer->enableTexture(true);
  m_enableshadows = false;
}

DemoApplication::~DemoApplication()
{
  if (m_shapeDrawer) {
    delete m_shapeDrawer;
  }
}

void DemoApplication::myinit(void)
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

  eyePos[m_forwardAxis] = -m_cameraDistance;

  btVector3 forward(eyePos[0], eyePos[1], eyePos[2]);

  if (forward.length2() < SIMD_EPSILON) {
    forward.setValue(1, 0, 0);
  }

  btVector3 right = m_cameraUp.cross(forward);
  btQuaternion roll(right, -rele);

  eyePos = btMatrix3x3(rot) * btMatrix3x3(roll) * eyePos;

  m_cameraPosition = eyePos + m_cameraTargetPosition;

  if (m_glutScreenWidth == 0 && m_glutScreenHeight == 0) {
    return;
  }

  btScalar aspect;
  btVector3 extents;

  if (m_glutScreenWidth > m_glutScreenHeight) {
    aspect = m_glutScreenWidth / static_cast<btScalar>(m_glutScreenHeight);
    extents.setValue(aspect * 1.0, 1.0, 0);
  } else {
    aspect = m_glutScreenHeight / static_cast<btScalar>(m_glutScreenWidth);
    extents.setValue(1.0, aspect * 1.0, 0);
  }


  if (m_ortho) {
    // reset matrix
    glLoadIdentity();

    extents *= m_cameraDistance;
    btVector3 lower = m_cameraTargetPosition - extents;
    btVector3 upper = m_cameraTargetPosition + extents;

    glOrtho(lower.getX(), upper.getX(), lower.getY(), upper.getY(), -1000, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  } else {
    if (m_glutScreenWidth > m_glutScreenHeight) {
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
}

void DemoApplication::moveCamera(float deltaAzi, float deltaEle)
{
  m_azi += deltaAzi;
  m_ele += deltaEle;
  while (m_azi < 0) {
    m_azi += 360;
  }
  while (m_azi >= 360) {
    m_azi -= 360;
  }
  while (m_ele < 0) {
    m_ele += 360;
  }
  while (m_ele >= 360) {
    m_ele -= 360;
  }
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
  m_glutScreenWidth = w;
  m_glutScreenHeight = h;

  glViewport(0, 0, w, h);
  updateCamera();
}

void DemoApplication::keyboardCallback(unsigned char key, int /*x*/, int /*y*/)
{
  switch (key) {
    case 'q': exit(0);                 break;
    case 'l': stepLeft();              break;
    case 'r': stepRight();             break;
    case 'f': stepFront();             break;
    case 'b': stepBack();              break;
    case 'z': zoomIn();                break;
    case 'x': zoomOut();               break;
    case 'i': toggle(m_idle);          break;
    case 'g': toggle(m_enableshadows); break;
    case 'o': toggle(m_ortho);         break;
    case 's': clientMoveAndDisplay();  break;
    case ' ': clientResetScene();      break;

    case 'w': toggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);        break;
    case 'a': toggleDebugFlag(btIDebugDraw::DBG_DrawAabb);             break;
    case 'c': toggleDebugFlag(btIDebugDraw::DBG_DrawContactPoints);    break;
    case 'C': toggleDebugFlag(btIDebugDraw::DBG_DrawConstraints);      break;
    case 'L': toggleDebugFlag(btIDebugDraw::DBG_DrawConstraintLimits); break;
    case 'd': toggleDebugFlag(btIDebugDraw::DBG_NoDeactivation);       break;

    default: break;
  }
}

void DemoApplication::toggleDebugFlag(int modeFlag)
{
  if (m_debugMode & modeFlag) {
    m_debugMode = m_debugMode & (~modeFlag);
  } else {
    m_debugMode |= modeFlag;
  }

  if (getDynamicsWorld() && getDynamicsWorld()->getDebugDrawer()) {
    getDynamicsWorld()->getDebugDrawer()->setDebugMode(m_debugMode);
  }
}

void DemoApplication::moveAndDisplay()
{
  if (!m_idle) {
    clientMoveAndDisplay();
  } else {
    displayCallback();
  }
}

int gPickingConstraintId = 0;
btVector3 gOldPickingPos;
btVector3 gHitPos(-1, -1, -1);
float gOldPickingDist  = 0.0;
btRigidBody* pickedBody = 0;//for deactivation state

btVector3 DemoApplication::getRayTo(int x, int y)
{



  if (m_ortho) {

    btScalar aspect;
    btVector3 extents;
    if (m_glutScreenWidth > m_glutScreenHeight) {
      aspect = m_glutScreenWidth / (btScalar)m_glutScreenHeight;
      extents.setValue(aspect * 1.0, 1.0, 0);
    } else {
      aspect = m_glutScreenHeight / (btScalar)m_glutScreenWidth;
      extents.setValue(1.0, aspect * 1.0, 0);
    }

    extents *= m_cameraDistance;
    btVector3 lower = m_cameraTargetPosition - extents;
    btVector3 upper = m_cameraTargetPosition + extents;

    btScalar u = x / btScalar(m_glutScreenWidth);
    btScalar v = (m_glutScreenHeight - y) / btScalar(m_glutScreenHeight);

    btVector3 p(0, 0, 0);
    p.setValue((1.0 - u) * lower.getX() + u * upper.getX(), (1.0 - v) * lower.getY() + v * upper.getY(), m_cameraTargetPosition.getZ());
    return p;
  }

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

  if (m_glutScreenWidth > m_glutScreenHeight) {
    aspect = m_glutScreenWidth / (btScalar)m_glutScreenHeight;

    hor *= aspect;
  } else {
    aspect = m_glutScreenHeight / (btScalar)m_glutScreenWidth;
    vertical *= aspect;
  }


  btVector3 rayToCenter = rayFrom + rayForward;
  btVector3 dHor = hor * 1.0 / float(m_glutScreenWidth);
  btVector3 dVert = vertical * 1.0 / float(m_glutScreenHeight);


  btVector3 rayTo = rayToCenter - 0.5 * hor + 0.5 * vertical;
  rayTo += btScalar(x) * dHor;
  rayTo -= btScalar(y) * dVert;
  return rayTo;
}

void DemoApplication::mouseFunc(int button, int state, int x, int y)
{
  if (state == 0) {
    m_mouseButtons |= 1 << button;
  } else {
    m_mouseButtons = 0;
  }

  m_mouseOldX = x;
  m_mouseOldY = y;

  updateModifierKeys();
  if ((m_modifierKeys & BT_ACTIVE_ALT) && (state == 0)) {
    return;
  }

  btVector3 rayTo = getRayTo(x, y);

  if (!m_dynamicsWorld) {
    return;
  }

  switch (button) {
    case 2:
      break; // Right-click
    case 1:
      break; // Middle-click

    case 0: {      // Left-click: add a point to point constraint for picking
      if (state == 0) {
        btVector3 rayFrom;

        if (m_ortho) {
          rayFrom = rayTo;
          rayFrom.setZ(-100.0);
        } else {
          rayFrom = m_cameraPosition;
        }

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

          pickedBody->forceActivationState(ACTIVE_TAG);
          pickedBody->setDeactivationTime(0.0);
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

      if (m_ortho) {
        newPivotB = oldPivotInB;
        newPivotB.setX(newRayTo.getX());
        newPivotB.setY(newRayTo.getY());
      } else {
        rayFrom = m_cameraPosition;
        btVector3 dir = newRayTo - rayFrom;
        dir.normalize();
        dir *= gOldPickingDist;
        newPivotB = rayFrom + dir;
      }

      pickCon->setPivotB(newPivotB);
    }
  }

  float dx, dy;
  dx = btScalar(x) - m_mouseOldX;
  dy = btScalar(y) - m_mouseOldY;

  // only if ALT key is pressed (Maya style)
  if (m_modifierKeys & BT_ACTIVE_ALT) {
    if (m_mouseButtons & 2) {
      btVector3 hor = getRayTo(0, 0) - getRayTo(1, 0);
      btVector3 vert = getRayTo(0, 0) - getRayTo(0, 1);
      btScalar multiplierX = btScalar(0.01);
      btScalar multiplierY = btScalar(0.01);
      if (m_ortho) {
        multiplierX = 1;
        multiplierY = 1;
      }

      m_cameraTargetPosition += hor * dx * multiplierX;
      m_cameraTargetPosition += vert * dy * multiplierY;

    } else if (m_mouseButtons & 1) {
      m_azi += dx * btScalar(0.2);
      m_azi = fmodf(m_azi, btScalar(360.0));
      m_ele += dy * btScalar(0.2);
      m_ele = fmodf(m_ele, btScalar(180.0));

    } else if (m_mouseButtons & 4) {
      m_cameraDistance -= dy * btScalar(0.2);
      if (m_cameraDistance < btScalar(0.1)) {
        m_cameraDistance = btScalar(0.1);
      }
    }
  }

  m_mouseOldX = x;
  m_mouseOldY = y;
  updateCamera();
}

btRigidBody* DemoApplication::localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape)
{
  btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

  // rigidbody is dynamic if and only if mass is non zero, otherwise static
  bool isDynamic = (mass != 0.0);

  btVector3 localInertia(0, 0, 0);
  if (isDynamic) {
    shape->calculateLocalInertia(mass, localInertia);
  }

  btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
  btRigidBody* body = new btRigidBody(cInfo);
  body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

  m_dynamicsWorld->addRigidBody(body);

  return body;
}

// See http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
void DemoApplication::setOrthographicProjection()
{
  // switch to projection mode
  glMatrixMode(GL_PROJECTION);

  // save previous matrix which contains the
  // settings for the perspective projection
  glPushMatrix();
  // reset matrix
  glLoadIdentity();
  // set a 2D orthographic projection
  gluOrtho2D(0, m_glutScreenWidth, 0, m_glutScreenHeight);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // invert the y axis, down is positive
  glScalef(1, -1, 1);
  // mover the origin from the bottom left corner
  // to the upper left corner
  glTranslatef(btScalar(0), btScalar(-m_glutScreenHeight), btScalar(0));

}

void DemoApplication::resetPerspectiveProjection()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  updateCamera();
}



//
void  DemoApplication::renderscene(int pass)
{
  btScalar  m[16];
  btMatrix3x3 rot;
  rot.setIdentity();
  const int numObj = m_dynamicsWorld->getNumCollisionObjects();
  btVector3 wireColor(1, 0, 0);
  for(int i = 0; i < numObj; i++) {
    btCollisionObject*  colObj = m_dynamicsWorld->getCollisionObjectArray()[i];
    btRigidBody*    body = btRigidBody::upcast(colObj);
    if(body && body->getMotionState()) {
      btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
      myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
      rot = myMotionState->m_graphicsWorldTrans.getBasis();
    } else {
      colObj->getWorldTransform().getOpenGLMatrix(m);
      rot = colObj->getWorldTransform().getBasis();
    }
    wireColor = btVector3(1.0, 1.0, 0.5); //wants deactivation
    if(i & 1) {
      wireColor = btVector3(0.0, 0.0, 1.0);
    }
    ///color differently for active, sleeping, wantsdeactivation states
    if (colObj->getActivationState() == 1) { //active
      if (i & 1) {
        wireColor += btVector3 (1.0, 0.0, 0.0);
      } else {
        wireColor += btVector3 (.5f, 0.0, 0.0);
      }
    }
    if(colObj->getActivationState() == 2) { //ISLAND_SLEEPING
      if(i & 1) {
        wireColor += btVector3(0.0, 1.0, 0.0);
      } else {
        wireColor += btVector3(0.0, 0.5, 0.0);
      }
    }

    btVector3 aabbMin, aabbMax;
    m_dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin, aabbMax);

    aabbMin -= btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
    aabbMax += btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);

    if (!(m_debugMode & btIDebugDraw::DBG_DrawWireframe)) {
      switch(pass) {
        case  0:
          m_shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, m_debugMode, aabbMin, aabbMax);
          break;
        case  1:
          m_shapeDrawer->drawShadow(m, m_sundirection * rot, colObj->getCollisionShape(), aabbMin, aabbMax);
          break;
        case  2:
          m_shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor * btScalar(0.3), 0, aabbMin, aabbMax);
          break;
      }
    }
  }
}

//
void DemoApplication::renderme()
{
  myinit();

  updateCamera();

  if (m_dynamicsWorld) {
    if(m_enableshadows) {
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
      glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFFL );
      glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
      glDisable(GL_LIGHTING);
      renderscene(2);
      glEnable(GL_LIGHTING);
      glDepthFunc(GL_LESS);
      glDisable(GL_STENCIL_TEST);
      glDisable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
      renderscene(0);
    }
  }

  updateCamera();

}
#include "BulletCollision/BroadphaseCollision/btAxisSweep3.h"

void  DemoApplication::clientResetScene()
{

  gNumClampedCcdMotions = 0;
  int numObj = 0;
  int i;

  if (m_dynamicsWorld) {
    numObj = m_dynamicsWorld->getNumCollisionObjects();

    ///create a copy of the array, not a reference!
    btCollisionObjectArray copyArray = m_dynamicsWorld->getCollisionObjectArray();




    for (i = 0; i < numObjects; i++) {
      btCollisionObject* colObj = copyArray[i];
      btRigidBody* body = btRigidBody::upcast(colObj);
      if (body) {
        if (body->getMotionState()) {
          btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
          myMotionState->m_graphicsWorldTrans = myMotionState->m_startWorldTrans;
          body->setCenterOfMassTransform( myMotionState->m_graphicsWorldTrans );
          colObj->setInterpolationWorldTransform( myMotionState->m_startWorldTrans );
          colObj->forceActivationState(ACTIVE_TAG);
          colObj->activate();
          colObj->setDeactivationTime(0);
          //colObj->setActivationState(WANTS_DEACTIVATION);
        }
        //removed cached contact points (this is not necessary if all objects have been removed from the dynamics world)
        //m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(colObj->getBroadphaseHandle(),getDynamicsWorld()->getDispatcher());

        body = btRigidBody::upcast(colObj);
        if (body && !body->isStaticObject()) {
          btRigidBody::upcast(colObj)->setLinearVelocity(btVector3(0, 0, 0));
          btRigidBody::upcast(colObj)->setAngularVelocity(btVector3(0, 0, 0));
        }
      }

    }

    ///reset some internal cached data in the broadphase
    m_dynamicsWorld->getBroadphase()->resetPool(getDynamicsWorld()->getDispatcher());
    m_dynamicsWorld->getConstraintSolver()->reset();

  }

}
