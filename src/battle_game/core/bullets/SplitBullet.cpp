//
// Created by 13552 on 2023/1/8.
//
#pragma once
#include "SplitBullet.h"

#include "battle_game/core/bullet.h"
#include "battle_game/core/bullets/cannon_ball.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
namespace battle_game::bullet {
SplitBullet::SplitBullet(GameCore *core,
                         uint32_t id,
                         uint32_t unit_id,
                         uint32_t player_id,
                         glm::vec2 position,
                         float rotation,
                         float damage_scale,
                         glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity) {
  split_countdown_ = 20;
}
void SplitBullet::Update() {
  split_countdown_--;
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == player_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
      should_die = true;
    }
    if (split_countdown_ == 0) {
      for (int i = -2; i <= 2; i++) {
        game_core_->PushEventGenerateBullet<bullet::CannonBall>(
            unit_id_, player_id_, position_, rotation_, damage_scale_,
            Rotate(velocity_, float(i * 1.0 / 10)));
      }
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}
void SplitBullet::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  SetColor(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f * split_countdown_ / 20});
  SetScale(glm::vec2{0.1f, 0.5f});
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}

SplitBullet::~SplitBullet() noexcept {
  if (split_countdown_) {
    for (int i = 0; i < 5; i++) {
      game_core_->PushEventGenerateParticle<particle::Smoke>(
          position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
          glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
    }
  }
}
}  // namespace battle_game::bullet
