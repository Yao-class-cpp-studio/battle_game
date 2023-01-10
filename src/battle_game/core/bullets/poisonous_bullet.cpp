#include "battle_game/core/bullets/poisonous_bullet.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
PoisonousBullet::PoisonousBullet(GameCore *core,
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

void PoisonousBullet::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.5f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/poison.png");
  DrawModel(0);
}

void PoisonousBullet::Update() {
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
      float damage_scale_ = damage_rate;
      damage_scale_ *= unit.second->GetMaxHealth();
      if(using_current_health) 
        damage_scale_ *= unit.second->GetHealth();
      int actual_damage = (int) damage_scale_;
      if(is_round_down) {
        if(damage_scale_ != actual_damage) 
          actual_damage = (int) damage_scale_ + 1;
      }
      game_core_->PushEventDealDamage(unit.first, id_, actual_damage);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

PoisonousBullet::~PoisonousBullet() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
