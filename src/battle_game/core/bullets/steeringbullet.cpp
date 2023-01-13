#include "battle_game/core/bullets/steeringbullet.h"

#include "battle_game/core/bullets/cannon_ball.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
namespace battle_game::bullet {
SteerBullet::SteerBullet(GameCore *core,
                         uint32_t id,
                         uint32_t unit_id,
                         uint32_t player_id,
                         glm::vec2 position,
                         float rotation,
                         float damage_scale,
                         glm::vec2 velocity)
    : CannonBall(core,
                 id,
                 unit_id,
                 player_id,
                 position,
                 rotation,
                 damage_scale,
                 velocity),
      velocity_(velocity) {
}

void SteerBullet::Update() {
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
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
      unit.second->SetRotation(game_core_->GetUnit(unit.first)->GetRotation() +
                               0.7853981634);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}
SteerBullet::~SteerBullet() {
}

}  // namespace battle_game::bullet
