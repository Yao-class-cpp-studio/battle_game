#include "battle_game/core/obstacles/moveable_block.h"

namespace battle_game::obstacle {

MoveableBlock::MoveableBlock(GameCore *game_core,
                             uint32_t id,
                             glm::vec2 position,
                             float rotation,
                             glm::vec2 scale)
    : Block::Block(game_core, id, position, rotation, scale) {
}

bool MoveableBlock::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

void MoveableBlock::BlockMove(glm::vec2 p) {
  position_ = position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                                glm::vec3{0.0f, 0.0f, 1.0f}) *
                                    glm::vec4{0.0f}};
}

}  // namespace battle_game::obstacle
