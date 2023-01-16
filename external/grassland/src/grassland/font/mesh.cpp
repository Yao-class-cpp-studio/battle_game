#include "grassland/font/mesh.h"

#include "map"

namespace grassland::font {

Mesh::Mesh() {
}

Mesh::Mesh(const std::vector<glm::vec2> &vertices,
           const std::vector<uint32_t> &indices,
           float advection) {
  this->vertices = vertices;
  this->indices = indices;
  this->advection = advection;
}

Mesh::Mesh(const std::vector<glm::vec2> &triangle_vertices, float advection) {
  this->advection = advection;

  class Vec2Eq {
   public:
    bool operator()(const glm::vec2 &a, const glm::vec2 &b) const {
      return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
  };
  std::map<glm::vec2, uint32_t, Vec2Eq> vec_index;
  auto vec_2_index = [&](const glm::vec2 &v) {
    if (!vec_index.count(v)) {
      uint32_t index = vertices.size();
      vec_index.insert(std::make_pair(v, index));
      vertices.push_back(v);
      return index;
    }
    return vec_index.at(v);
  };
  vertices.clear();
  indices.clear();
  for (auto &v : triangle_vertices) {
    indices.push_back(vec_2_index(v));
  }
}

std::vector<glm::vec2> &Mesh::GetVertices() {
  return vertices;
}

const std::vector<glm::vec2> &Mesh::GetVertices() const {
  return vertices;
}

std::vector<uint32_t> &Mesh::GetIndices() {
  return indices;
}

const std::vector<uint32_t> &Mesh::GetIndices() const {
  return indices;
}

float Mesh::GetAdvection() const {
  return advection;
}

float &Mesh::GetAdvection() {
  return advection;
}
}  // namespace grassland::font
