#include "battle_game/core/bullets/rocket.h"

#include <cmath>

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
#define PI 3.141592653

namespace battle_game::bullet {
Rocket::Rocket(GameCore *core,
               uint32_t id,
               uint32_t unit_id,
               uint32_t player_id,
               glm::vec2 position,
               float rotation,
               float damage_scale,
               glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity) {
  auto &units = game_core_->GetUnits();
  auto player = game_core_->GetPlayer(player_id_);
  auto &input_data = player->GetInputData();
  auto where_cursor_is = input_data.mouse_cursor_position;
  float distance = 1e30;
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    auto diff = where_cursor_is - unit.second->GetPosition();
    float length = sqrt(diff.x * diff.x + diff.y * diff.y);
    if (length < distance) {
      distance = length;
      player_locked_ = unit.first;
    }
  }
}

void Rocket::Render() {
  SetTransformation(position_, rotation_, glm::vec2{1.0f});
  SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
  SetTexture("../../textures/rocket.png");
  DrawModel(0);
}

void Rocket::Update() {
  auto &units = game_core_->GetUnits();
  if (units.find(player_locked_) == units.end()) {
    game_core_->PushEventRemoveBullet(id_);
    return;
  }
  if (harmful_ < 20)
    harmful_ *= 1.02;
  glm::vec2 diff;
  for (auto &unit : units) {
    if (unit.first == player_locked_) {
      glm::vec2 nowpos = unit.second->GetPosition();
      glm::vec2 whereIam = this->GetPosition();
      diff = glm::vec2{nowpos.x - whereIam.x, nowpos.y - whereIam.y};
      float length = sqrt(diff.x * diff.x + diff.y * diff.y);
      diff = glm::vec2{0.25 * harmful_ * diff.x / length,
                       0.25 * harmful_ * diff.y / length};
    }
  }
  velocity_ = diff;
  position_ += velocity_ * kSecondPerTick;
  float angle = 0;
  if (diff.x < 0) {
    if (diff.y < 0) {
      angle = PI / 2 + atan(abs(diff.y) / abs(diff.x));
    } else if (diff.y == 0) {
      angle = PI / 2;
    } else {
      angle = atan(abs(diff.x) / abs(diff.y));
    }
  } else if (diff.x == 0) {
    if (diff.y >= 0) {
      angle = 0;
    } else {
      angle = PI;
    }
  } else {
    if (diff.y > 0) {
      angle = PI * 2 - atan(abs(diff.x) / abs(diff.y));
    } else if (diff.y == 0) {
      angle = 1.5 * PI;
    } else {
      angle = PI + atan(abs(diff.x) / abs(diff.y));
    }
  }
  rotation_ = angle;
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_,
                                      damage_scale_ * harmful_);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

Rocket::~Rocket() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
