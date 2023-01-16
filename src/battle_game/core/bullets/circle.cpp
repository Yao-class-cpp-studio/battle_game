#include "battle_game/core/bullets/circle.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
Circle::Circle(GameCore *core,
                   uint32_t id,
                   uint32_t unit_id,
                   uint32_t player_id,
                   glm::vec2 position,
                   float rotation,
                   float damage_scale,
                   float radius)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      radius_(radius) {
}

void Circle::Render() {
  SetTransformation(position_, rotation_, glm::vec2{radius_});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}

void Circle::Update() {
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    auto pos = unit.second->WorldToLocal(position_);
    float distance = pos.x * pos.x + pos.y * pos.y;
    if (distance > radius_ * radius_)
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 0.10f);
  }
}

Circle::~Circle() {}
}  // namespace battle_game::bullet
