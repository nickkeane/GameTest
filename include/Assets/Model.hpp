//
// Created by Nick on 5/29/2026.
//

#ifndef CONANFILE_PY_MODEL_HPP
#define CONANFILE_PY_MODEL_HPP
#include <cstdint>

#include <vector>

class Model {
public:
  void Load(std::string_view const& szFile);
private:
  std::vector<int64_t> marrIndices;
  std::vector<glm::vec4f> marrVertices;
  std::vector<glm::vec4f> marrNormals;
  std::vector<glm::vec2f> marrTexCoords;
  std::vector<glm::vec3f> marrTangents;
  std::vector<glm::vec3f> marrBitangents;
};

#endif  // CONANFILE_PY_MODEL_HPP
