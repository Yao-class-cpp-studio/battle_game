#pragma once
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/obstacle.h"
#include "battle_game/core/obstacles/destructible_block.h"

namespace battle_game::obstacle {
class FragileBlock : public DestructibleBlock {
 public:
  FragileBlock(GameCore *game_core,
               uint32_t id,
               glm::vec2 position,
               float rotation = 0.0f,
               glm::vec2 scale = glm::vec2{1.0f, 1.0f},
               float damage_scale = 1.0f,
               uint32_t split_num = 4,
               glm::vec2 velocity = glm::vec2{0.0f, 20.0f});
  ~FragileBlock();

 private:
  uint32_t split_num_{4};
  float damage_scale_{1.0f};
  glm::vec2 velocity_{0.0f, 20.0f};
};
}  // namespace battle_game::obstacle
