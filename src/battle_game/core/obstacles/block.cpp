#include "battle_game/core/obstacles/block.h"

namespace battle_game::obstacle {

Block::Block(GameCore *game_core,
             uint32_t id,
             glm::vec2 position,
             float rotation,
             glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
}

bool Block::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

void Block::handle_collision(glm::vec2 &p) const {
  auto local_p = WorldToLocal(p);
  auto del1 = scale_.x - local_p.x;
  auto del2 = local_p.x + scale_.x;
  auto del3 = scale_.y - local_p.y;
  auto del4 = local_p.y + scale_.y;
  if (del1 <= del2 && del1 <= del3 && del1 <= del4) {
    local_p.x = scale_.x;
  } else if (del2 <= del1 && del2 <= del3 && del2 <= del4) {
    local_p.x = -scale_.x;
  } else if (del3 <= del1 && del3 <= del2 && del3 <= del4) {
    local_p.y = scale_.y;
  } else {
    local_p.y = -scale_.y;
  }

  p = LocalToWorld(local_p);
}

void Block::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
