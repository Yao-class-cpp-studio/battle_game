#include "battle_game/core/bullets/missile.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
Missile::Missile(GameCore *core,
                 uint32_t id,
                 uint32_t unit_id,
                 uint32_t player_id,
                 glm::vec2 position,
                 float rotation,
                 float damage_scale,
                 glm::vec2 velocity,
                 float max_velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      max_velocity_(max_velocity) {
}

float Missile::GetMaxVelocity() {
  return max_velocity_;
}

void Missile::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.5f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}

void Missile::Update() {
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
    if (unit.second->GetPlayerId() == player_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
      should_die = true;
    }
  }

  if (!should_die) {
    glm::vec2 best_diff = {160.0f, 160.0f};
    float best_cost = CalcCost(best_diff);
    for (auto &unit : units) {
      if (unit.first == player_id_) {
        continue;
      }
      auto target_pos = unit.second->GetPosition();
      auto diff = target_pos - position_;
      if (glm::length(diff) > 8.0f) {
        continue;
      }
      float cost = CalcCost(best_diff);
      if (cost < best_cost) {
        best_cost = cost;
        best_diff = diff;
      }
    }
    auto fix = CalcFix(best_diff);
    velocity_ =
        (1 - resistance_ * glm::length(velocity_) / max_velocity_) * velocity_ +
        fix;
  } else {
    game_core_->PushEventRemoveBullet(id_);
  }
}

float Missile::CalcCost(glm::vec2 diff) {
}

glm::vec2 Missile::CalcFix(glm::vec2 diff) {
  float distance = glm::length(diff);
  glm::normalize(diff);
  float speed = glm::length(velocity_);
  auto v = velocity_;
  glm::normalize(v);
  if (speed < 1e-4)
    return resistance_ * max_velocity_ * diff;
}

Missile::~Missile() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet