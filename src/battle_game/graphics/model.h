#pragma once
#include "battle_game/graphics/util.h"
#include "vector"

namespace battle_game {
class Model {
 public:
  Model() = default;
  Model(const std::vector<ObjectVertex> &vertices,
        const std::vector<uint32_t> &indices);
  std::vector<ObjectVertex> &GetVertices() {
    return vertices_;
  }
  [[nodiscard]] const std::vector<ObjectVertex> &GetVertices() const {
    return vertices_;
  }
  std::vector<uint32_t> &GetIndices() {
    return indices_;
  }
  [[nodiscard]] const std::vector<uint32_t> &GetIndices() const {
    return indices_;
  }

 private:
  std::vector<ObjectVertex> vertices_;
  std::vector<uint32_t> indices_;
};
}  // namespace battle_game
