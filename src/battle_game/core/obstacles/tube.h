#pragma once
#include "battle_game/core/obstacle.h"

namespace battle_game::obstacle {
class Tube : public Obstacle {
 public:
  Tube(GameCore *game_core,
       uint32_t id,
       glm::vec2 position,
       float rotation = 0.0f,
       glm::vec2 scale = glm::vec2{1.0f, 1.0f});

 private:
  [[nodiscard]] bool IsBlocked(glm::vec2 p) const override;
  void Render() override;
  glm::vec2 scale_{1.0f};
  bool ver_;
};
}  // namespace battle_game::obstacle
