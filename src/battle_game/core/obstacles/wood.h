#pragma once
#include "battle_game/core/obstacle.h"

namespace battle_game::obstacle {
class Wood : public Obstacle {
 public:
  Wood(GameCore *game_core,
       uint32_t id,
       glm::vec2 position,
       float rotation = 0.0f,
       glm::vec2 scale = glm::vec2{1.0f, 1.0f});
  ~Wood();

 private:
  [[nodiscard]] bool IsBlocked(glm::vec2 p) const override;
  void Render() override;
  glm::vec2 scale_{1.0f};
};
}  // namespace battle_game::obstacle
