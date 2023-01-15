#include "battle_game/core/obstacles/crater.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/object.h"
#include "battle_game/core/obstacle.h"
#include "battle_game/core/obstacles/obstacles.h"

namespace battle_game::obstacle {

Crater::Crater(GameCore *core,
             uint32_t id,
             glm::vec2 position,
             float rotation,
             glm::vec2 scale)
    : Obstacle(core, id, position, rotation) {
}

bool Crater::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

void Crater::Render() { 
  battle_game::SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 0.9f});
  battle_game::SetTexture("../../textures/crater.png");
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
  if (recovery_count_down_)
    recovery_count_down_--;
  else {
    RecoverRender();
  }
}

void Crater::RecoverRender() {
    battle_game::SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 0.9f});
    battle_game::SetTexture("../../textures/mushroom_cloud.png");
    battle_game::SetTransformation(position_, rotation_, scale_);
    battle_game::DrawModel(0);
}



  void Crater::Update() {
  Recover();
  }

void Crater::Recover() {
  auto &units = game_core_->GetObstacles();
  if (crater_count_down_) {
    crater_count_down_--;
  } else {

      for (auto &unit : units) {
        if (unit.first == id_) {
          game_core_->PushEventRemoveObstacle(unit.first);
        }
      }
      crater_count_down_ = 3 * kTickPerSecond;  
  }
}


}  // namespace battle_game::obstacle
