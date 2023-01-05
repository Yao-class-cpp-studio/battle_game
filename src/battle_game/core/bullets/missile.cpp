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
      velocity_(velocity) {
  max_velocity_ = std::max(1.0f, max_velocity);
}

float Missile::GetMaxVelocity() {
  return max_velocity_;
}

void Missile::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.2f});
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
    glm::vec2 best_diff = velocity_;
    float best_cost = 500.0f;
    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        continue;
      }
      if (unit.second->GetPlayerId() == player_id_) {
        continue;
      }
      auto target_pos = unit.second->GetPosition();
      auto diff = target_pos - position_;

      if (glm::length(diff) > 12.0f) {
        continue;
      }
      if (glm::length(diff) < 1.0f) {
        game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
        game_core_->PushEventRemoveBullet(id_);
        return;
      }
      float cost = CalcCost(diff);
      if (cost < best_cost) {
        best_cost = cost;
        best_diff = diff;
      }
    }
    auto fix = CalcFix(best_diff);

    velocity_ *= (1 - resistance_ * glm::length(velocity_) / max_velocity_);
    velocity_ += fix;
    auto speed = glm::length(velocity_);
    if (speed > max_velocity_) {
      velocity_ = glm::normalize(velocity_) * max_velocity_;
    }

  } else {
    game_core_->PushEventRemoveBullet(id_);
  }
}

float Missile::CalcCost(glm::vec2 diff) {
  float distance = glm::length(diff);

  diff = glm::normalize(diff);
  float speed = glm::length(velocity_);
  if (speed < 1e-3) {
    return distance;
  }
  auto v = glm::normalize(velocity_);
  auto angel = glm::acos(glm::dot(diff, v));
  return distance * (1 - speed * glm::cos(angel) / max_velocity_);
}

glm::vec2 Missile::CalcFix(glm::vec2 diff) {
  float distance = glm::length(diff);
  diff = glm::normalize(diff);
  float speed = glm::length(velocity_);
  if (speed < 1e-3) {
    return 2 * resistance_ * max_velocity_ * diff;
  }
  auto v = glm::normalize(velocity_);
  auto dot = glm::dot(diff, v);
  if (dot > 1 && dot < 1 + 1e-3) {
    dot = 1;
  }
  auto angel = glm::acos(dot);
  auto fangel = std::atan2(resistance_ * max_velocity_,
                           (1 - resistance_ * speed / max_velocity_) * speed);
  if (angel < fangel) {
    return resistance_ * max_velocity_ * diff;
  }
  auto per = Rotate(v, glm::radians(90.0f));
  auto rad = glm::radians(60.0f);
  if (glm::dot(per, diff) < 0) {
    rad = -rad;
  }

  return Rotate(resistance_ * max_velocity_ * v, rad) +
         glm::sin(angel) * resistance_ * max_velocity_ * v;
}

Missile::~Missile() {
  for (int i = 0; i < 6; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 3.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet