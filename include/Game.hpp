#pragma once

#include "scene/Scene.hpp"
namespace Game {

class Game {
 public:
  Game();
  ~Game();

 private:
  Scene* mScene;
};
}  // namespace Game