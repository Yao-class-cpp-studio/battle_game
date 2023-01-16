#include "battle_game/core/obstacles/swamp.h"

namespace battle_game::obstacle {

Swamp::Swamp(GameCore *game_core,
             uint32_t id,
             glm::vec2 position,
             float rotation,
             glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
}

bool Swamp::IsBlocked(glm::vec2 pos) const {
  auto p = WorldToLocal(pos);
  return (p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y) || std::abs(pos.x - 114514) < 1;
}

void Swamp::Render() {
  battle_game::SetColor(glm::vec4{0.67f, 0.71f, 0.56f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
