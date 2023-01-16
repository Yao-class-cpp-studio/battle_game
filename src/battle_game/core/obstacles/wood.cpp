#include "battle_game/core/obstacles/wood.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"

namespace battle_game::obstacle {

Wood::Wood(GameCore *game_core,
           uint32_t id,
           glm::vec2 position,
           float rotation,
           glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
}

Wood::~Wood() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.5f,
        glm::vec4{0.4f, 0.2f, 0.0f, 4.0f}, 3.0f);
  }
}

bool Wood::IsBlocked(glm::vec2 p) const {
  auto pl = WorldToLocal(p);
  if (pl.x <= scale_.x && pl.x >= -scale_.x && pl.y <= scale_.y &&
      pl.y >= -scale_.y) {
    for (auto &bullet : game_core_->GetBullets()) {
      std::string temp = bullet.second->type_;
      if (bullet.second->GetPosition() == p && temp == "Cannonball") {
        game_core_->PushEventRemoveObstacle(id_);
      }
      if (bullet.second->GetPosition() == p && temp == "Rocket") {
        game_core_->AddObstacle<obstacle::Block>(position_, rotation_);
        game_core_->PushEventRemoveObstacle(id_);
      }
    }
    return true;
  }
  return false;
}

void Wood::Render() {
  battle_game::SetColor(glm::vec4{0.8f, 0.4f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
