#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#ifdef BT_USE_DOUBLE_PRECISION
  #define btglLoadMatrix glLoadMatrixd
  #define btglMultMatrix glMultMatrixd
  #define btglColor3     glColor3d
  #define btglVertex3    glVertex3d
#else
  #define btglLoadMatrix glLoadMatrixf
  #define btglMultMatrix glMultMatrixf
  #define btglColor3     glColor3f
  #define btglVertex3    glVertex3f
#endif