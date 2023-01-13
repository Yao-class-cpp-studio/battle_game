#pragma once
#include "battle_game/core/obstacle.h"
#include "battle_game/core/units/defence_tank.h"
#include "block.h"

namespace battle_game::obstacle {
class MoveableBlock : public Block {
  friend unit::DefenceTank;

 public:
  MoveableBlock(GameCore *game_core,
                uint32_t id,
                glm::vec2 position,
                float rotation = 0.0f,
                glm::vec2 scale = glm::vec2{1.0f, 1.0f});

 private:
  // input the touching point
  void BlockMove(glm::vec2 p);
  [[nodiscard]] bool IsBlocked(glm::vec2 p) const override;
};
}  // namespace battle_game::obstacle
