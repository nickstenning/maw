#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

#include "GlutRunner.h"
#include "GlutDemoApplication.h"

void GlutDemoApplication::updateModifierKeys()
{
  m_modifierKeys = 0;

  if (glutGetModifiers() & GLUT_ACTIVE_ALT) {
    m_modifierKeys |= BT_ACTIVE_ALT;
  }

  if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
    m_modifierKeys |= BT_ACTIVE_CTRL;
  }

  if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
    m_modifierKeys |= BT_ACTIVE_SHIFT;
  }
}

void GlutDemoApplication::specialKeyboard(int key, int /*x*/, int /*y*/)
{
  switch (key) {
    case GLUT_KEY_LEFT:      stepLeft(); break;
    case GLUT_KEY_RIGHT:     stepRight(); break;
    case GLUT_KEY_UP:        stepFront(); break;
    case GLUT_KEY_DOWN:      stepBack(); break;
    case GLUT_KEY_PAGE_UP:   zoomIn(); break;
    case GLUT_KEY_PAGE_DOWN: zoomOut(); break;

    default: break;
  }

  glutPostRedisplay();
}

void GlutDemoApplication::clientMoveAndDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  btScalar timeStep = getDeltaTimeMicroseconds() * 0.000001f;

  if (m_dynamicsWorld) {
    m_dynamicsWorld->stepSimulation(timeStep);
    m_dynamicsWorld->debugDrawWorld();
  }

  render();

  glFlush();
  glutSwapBuffers();
}

void GlutDemoApplication::displayCallback()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_dynamicsWorld) {
    m_dynamicsWorld->debugDrawWorld();
  }

  render();

  glFlush();
  glutSwapBuffers();
}

