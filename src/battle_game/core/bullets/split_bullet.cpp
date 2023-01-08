#include "battle_game/core/bullets/split_bullet.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
#include "battle_game/graphics/graphics.h"


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
        ResetCountDown();
}

void SplitBullet::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}

void SplitBullet::ResetCountDown () {
    split_count_down_ = split_interval;
}

void SplitBullet::Update() {
  position_ += velocity_ * kSecondPerTick;

  if (split_count_down_) {
    split_count_down_--;
  } else {
    ResetCountDown();
    auto offset = glm::radians(10.0f);
    auto &units = game_core_->GetUnits();
      for (auto &unit : units) {
        if (unit.first == unit_id_) {
          unit.second->GenerateBullet<bullet::SplitBullet>(
            position_ , rotation_ + offset ,
            damage_scale_, Rotate(velocity_, offset));
          unit.second->GenerateBullet<bullet::SplitBullet>(
            position_ , rotation_ - offset ,
            damage_scale_, Rotate(velocity_, -offset));
            break;
        }
      }
  }
  

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
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }

}

SplitBullet::~SplitBullet() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
