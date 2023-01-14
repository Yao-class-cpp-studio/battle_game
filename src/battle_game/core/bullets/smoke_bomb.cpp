#include "battle_game/core/bullets/smoke_bomb.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
SmokeBomb::SmokeBomb(GameCore *core,
                     uint32_t id,
                     uint32_t unit_id,
                     uint32_t player_id,
                     glm::vec2 position,
                     float rotation,
                     float damage_scale,
                     glm::vec2 target,
                     float radius,
                     float duration,
                     float damage_duration)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      target_(target),
      velocity_((target - position) * (1.0f / duration)),
      radius_(radius),
      duration_(duration * kTickPerSecond + 0.5f),
      damage_duration_(damage_duration * kTickPerSecond + 0.5f),
      current_time_(0u) {
}

void SmokeBomb::Render() {
  if (current_time_ < duration_) {
    SetTransformation(position_, rotation_, glm::vec2{0.5f});
    // SetColor(game_core_->GetPlayerColor(player_id_));
    SetTexture("../../textures/bomb.png");
    DrawModel(0);
  }
}

void SmokeBomb::Update() {
  current_time_++;
  if (current_time_ < duration_) {
    position_ += velocity_ * kSecondPerTick;
    rotation_ += 5.0f * kSecondPerTick;
  } else if (current_time_ < duration_ + damage_duration_ * 5) {
    position_ = target_;
    if (current_time_ == duration_) {
      game_core_->PushEventGenerateParticle<particle::Smoke>(
          target_, rotation_, glm::vec2{}, radius_,
          glm::vec4{0.0f, 0.0f, 0.0f, 1.0f},
          kTickPerSecond / (damage_duration_ * 8.0f));
    }
    if ((current_time_ - duration_) % damage_duration_ == 0) {
      auto &units = game_core_->GetUnits();
      float damage =
          damage_scale_ *
          (10.0f - 2.0f * ((current_time_ - duration_) / damage_duration_));
      for (auto &unit : units) {
        if (glm::length(unit.second->GetPosition() - position_) <= radius_) {
          if (unit.first == unit_id_) {
            game_core_->PushEventDealDamage(unit.first, id_, damage * 0.5f);
          } else {
            game_core_->PushEventDealDamage(unit.first, id_, damage);
          }
        }
      }
    }
  } else {
    game_core_->PushEventRemoveBullet(id_);
  }
}
}  // namespace battle_game::bullet
