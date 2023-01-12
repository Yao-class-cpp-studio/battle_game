#include "battle_game/core/bullets/udongein_directional_bullet.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
UdongeinDirectionalBullet::UdongeinDirectionalBullet(GameCore *core,
                                                     uint32_t id,
                                                     uint32_t unit_id,
                                                     uint32_t player_id,
                                                     glm::vec2 position,
                                                     float rotation,
                                                     float damage_scale,
                                                     glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity) {
}

void UdongeinDirectionalBullet::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.5f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/udongein_directional_bullet.png");
  DrawModel(0);
}

void UdongeinDirectionalBullet::Update() {
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 2.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

UdongeinDirectionalBullet::~UdongeinDirectionalBullet() {
}
}  // namespace battle_game::bullet
