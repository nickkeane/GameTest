#include "Game.hpp"

#include "scene/Scene.hpp"

namespace Game {
Game::Game() { mScene = new Scene(); }

Game::~Game() {}
}  // namespace Game