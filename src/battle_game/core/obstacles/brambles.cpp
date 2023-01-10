#include "battle_game/core/obstacles/brambles.h"
#include "battle_game/core/game_core.h"
namespace battle_game::obstacle {

Brambles::Brambles(GameCore *game_core,
             uint32_t id,
             glm::vec2 position,
             float rotation,
             glm::vec2 scale)
    : Obstacle(game_core, id, position, rotation) {
}

bool Brambles::IsBlocked(glm::vec2 p) const {
  return false;
}

bool Brambles::IsInBrambles(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

void Brambles::Update() {
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (IsInBrambles(unit.second->GetPosition()))
      game_core_->PushEventDealDamage(unit.first, id_, 0.1f);
  }

}

void Brambles::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.5f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
