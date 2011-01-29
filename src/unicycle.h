/*
 *  Unicycle.h
 *  maw
 *
 *  Created by Nick Stenning on 14/12/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include <BulletDynamics/btBulletDynamicsCommon.h>

class Unicycle {
public:
  Unicycle () {}
  ~Unicycle () {}

  Unicycle& position(btVector3 const& pos);
  btVector3& position() const;

  static const btScalar forkWidth;
  static const btScalar forkLength;
  static const btScalar wheelWidth;
  static const btScalar wheelRadius;
private:
  btVector3 const& m_position;
};
