#pragma once
#include "grassland/font/util.h"
#include "vector"

namespace grassland::font {
struct Mesh {
  Mesh();
  Mesh(const std::vector<glm::vec2> &vertices,
       const std::vector<uint32_t> &indices,
       float advection);
  Mesh(const std::vector<glm::vec2> &triangle_vertices, float advection);
  std::vector<glm::vec2> &GetVertices();
  [[nodiscard]] const std::vector<glm::vec2> &GetVertices() const;
  std::vector<uint32_t> &GetIndices();
  [[nodiscard]] const std::vector<uint32_t> &GetIndices() const;
  float &GetAdvection();
  [[nodiscard]] float GetAdvection() const;

  std::vector<glm::vec2> vertices;
  std::vector<uint32_t> indices;
  float advection{0.0f};
};
}  // namespace grassland::font
