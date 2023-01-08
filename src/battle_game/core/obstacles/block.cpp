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

float Block::vec2_cross(glm::vec2 a, glm::vec2 b) {
  return a.x * b.y - a.y * b.x;
}

bool Block::IsIntersect(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d) {
  if (std::max(c.x, d.x) < std::min(a.x, b.x) ||
      std::max(c.y, d.y) < std::min(a.y, b.y) ||
      std::max(a.x, b.x) < std::min(c.x, d.x) ||
      std::max(a.y, b.y) < std::min(c.y, d.y))
    return false;
  if (vec2_cross(a - d, c - d) * vec2_cross(b - d, c - d) > 0 ||
      vec2_cross(c - a, b - a) * vec2_cross(d - a, b - a) > 0)
    return false;
  return true;
}

std::pair<glm::vec2, glm::vec2> Block::GetSurfaceNormal(glm::vec2 origin, glm::vec2 terminus) {
  origin = WorldToLocal(origin);
  terminus = WorldToLocal(terminus);
  glm::vec2 intersection = glm::vec2(0, 0);
  glm::vec2 direction = glm::vec2(0, 0);
  if (IsIntersect(origin, terminus, glm::vec2(-scale_.x, -scale_.y),
                  glm::vec2(-scale_.x, scale_.y))) {
    intersection = origin + (terminus - origin) * (-scale_.x - origin.x) /
                                (terminus.x - origin.x);
    direction = glm::vec2(-1, 0);
  } else if (IsIntersect(origin, terminus, glm::vec2(scale_.x, -scale_.y),
                  glm::vec2(scale_.x, scale_.y))) {
    intersection = origin + (terminus - origin) * (scale_.x - origin.x) /
                                (terminus.x - origin.x);
    direction = glm::vec2(1, 0);
  } else if (IsIntersect(origin, terminus, glm::vec2(-scale_.x, -scale_.y),
                  glm::vec2(scale_.x, -scale_.y))) {
    intersection = origin + (terminus - origin) * (-scale_.y - origin.y) /
                                (terminus.y - origin.y);
    direction = glm::vec2(0, -1);
    ;
  } else if (IsIntersect(origin, terminus, glm::vec2(-scale_.x, scale_.y),
                  glm::vec2(scale_.x, scale_.y))) {
    intersection = origin + (terminus - origin) * (scale_.y - origin.y) /
                                (terminus.y - origin.y);
    direction = glm::vec2(0, 1);
  }
  if (direction != glm::vec2(0, 0)) {
    direction =
        LocalToWorld(intersection + direction) - LocalToWorld(intersection);
    direction /= sqrt(direction.x * direction.x + direction.y * direction.y);
    intersection = LocalToWorld(intersection);
  }
  return std::make_pair(intersection, direction);
}
void Block::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
