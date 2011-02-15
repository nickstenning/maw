#ifndef GL_DEBUG_DRAWER_H
#define GL_DEBUG_DRAWER_H

#include <LinearMath/btIDebugDraw.h>


class GLDebugDrawer : public btIDebugDraw
{
  int m_debugMode;

public:

  GLDebugDrawer();


  virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor);

  virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

  virtual void drawSphere (btScalar radius, const btTransform& trans, const btVector3& color) {
    btIDebugDraw::drawSphere(radius, trans, color);
  }

  virtual void drawSphere (const btVector3& p, btScalar radius, const btVector3& color);

  virtual void drawBox(const btVector3& a, const btVector3& b, const btTransform& t, const btVector3& c) {
    btIDebugDraw::drawBox(a, b, t, c);
  }

  virtual void drawBox(const btVector3& a, const btVector3& b, const btVector3& c) {
    btIDebugDraw::drawBox(a, b, c);
  }

  virtual void drawBox (const btVector3& boxMin, const btVector3& boxMax, const btVector3& color, btScalar alpha);


  virtual void drawTriangle(const btVector3& v1, const btVector3& v2, const btVector3& v3, const btVector3& v4, const btVector3& v5, const btVector3& v6, const btVector3& v7, btScalar s) {
    btIDebugDraw::drawTriangle(v1, v2, v3, v4, v5, v6, v7, s);
  }

  virtual void drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha);

  virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

  virtual void reportErrorWarning(const char* warningString);

  virtual void draw3dText(const btVector3& location,const char* textString);

  virtual void setDebugMode(int debugMode);

  virtual int getDebugMode() const { return m_debugMode; }

};

#endif //GL_DEBUG_DRAWER_H
