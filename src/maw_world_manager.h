#ifndef MAW_WORLD_MANAGER_H
#define MAW_WORLD_MANAGER_H

#include "world_manager.h"

class btTransform;

class MawWorldManager : public WorldManager
{
public:
  MawWorldManager();
  virtual ~MawWorldManager() {};

  void addUnicycle(btTransform const& pos);
};

#endif // MAW_WORLD_MANAGER_H