#include "battle_game/core/bullets/boomerang.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
Boomerang::Boomerang(GameCore *core,
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

void Boomerang::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.8f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/boomerang.png");
  DrawModel(0);
}

void Boomerang::Update() {
  time_ += 1;
  bool should_die = false;
  if (time_ <= 25)
    position_ += velocity_ * kSecondPerTick;
  else {
    if (time_ <= 50)
      position_ -= velocity_ * kSecondPerTick;
    else {
      return_ = true;
      should_die = true;
    }
  }
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  if (time_ <= 25 && hit_ == 0) {
    auto &units = game_core_->GetUnits();
    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        continue;
      }
      if (unit.second->IsHit(position_)) {
        game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
        hit_ += 1;
        game_core_->PushEventGenerateParticle<particle::Smoke>(
            position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
            glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
      }
    }
  }

  if (time_ >= 25) {
    auto &units = game_core_->GetUnits();
    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        continue;
      }
      if (unit.second->IsHit(position_)) {
        game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
        should_die = true;
      }
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

Boomerang::~Boomerang() {
  if (!return_) {
    for (int i = 0; i < 5; i++) {
      game_core_->PushEventGenerateParticle<particle::Smoke>(
          position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
          glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
    }
  }
}
}  // namespace battle_game::bullet
