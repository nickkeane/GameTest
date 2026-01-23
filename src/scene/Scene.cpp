#include <scene/Scene.hpp>

namespace Game {
Scene::Scene() {}
Scene::~Scene() {}

std::unique_ptr<void> const& Scene::getObject(std::string const& szName) const {
  return mmapObjects.at(szName);
}

}  // namespace Game