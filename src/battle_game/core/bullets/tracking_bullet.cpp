#include "battle_game/core/bullets/tracking_bullet.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
Tracking::Tracking(GameCore *core,
                   uint32_t id,
                   uint32_t unit_id,
                   uint32_t player_id,
                   glm::vec2 position,
                   float rotation,
                   float damage_scale,
                   glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      rotation_(rotation),
      tracking_duration_(600) {
}
void Tracking::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.8f});
  SetColor({0.0f, 0.0f, 0.0f, 1.0f});
  SetTexture("../../textures/particle5.png");
  DrawModel(0);
}
void Tracking::Update() {
  if (whole_timer_) {
    whole_timer_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      auto diff = input_data.mouse_cursor_position - position_;
      if (glm::length(diff) >= 0.5) {
        auto rotation = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
        for (int i = -20; i < 20; i++) {
          auto velocity =
              Rotate(glm::vec2{0.0f, 20.0f}, rotation + glm::radians(float(i)));
          game_core_->PushEventGenerateParticle<particle::Signal>(
              position_, rotation_, velocity, 0.2f,
              glm::vec4{0.0f, 0.3f, 0.6f, 0.1f}, 3.0f);
        }
      }
      whole_timer_ = 60;
      is_second = true;
      partial_timer_ = 10;
    }
  }
  if (!partial_timer_ && is_second) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      auto diff = input_data.mouse_cursor_position - position_;
      if (glm::length(diff) >= 0.5) {
        auto rotation = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
        for (int i = -20; i < 20; i++) {
          auto velocity =
              Rotate(glm::vec2{0.0f, 20.0f}, rotation + glm::radians(float(i)));
          game_core_->PushEventGenerateParticle<particle::Signal>(
              position_, rotation_, velocity, 0.2f,
              glm::vec4{0.0f, 0.3f, 0.6f, 0.1f}, 3.0f);
        }
      }
      is_second = false;
      is_third = true;
      partial_timer_ = 10;
    }
  }
  if (partial_timer_) {
    partial_timer_--;
  }
  if (!partial_timer_ && is_third) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      auto diff = input_data.mouse_cursor_position - position_;
      if (glm::length(diff) >= 0.5) {
        auto rotation = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
        for (int i = -20; i < 20; i++) {
          auto velocity =
              Rotate(glm::vec2{0.0f, 20.0f}, rotation + glm::radians(float(i)));
          game_core_->PushEventGenerateParticle<particle::Signal>(
              position_, rotation_, velocity, 0.2f,
              glm::vec4{0.0f, 0.3f, 0.6f, 0.1f}, 3.0f);
        }
      }
      is_third = false;
    }
  } else if (partial_timer_) {
    partial_timer_--;
  }
  position_ += velocity_ * kSecondPerTick;
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 0.3) {
      rotation_ = std::atan2(diff.y, diff.x);
      velocity_ = Rotate(glm::vec2{0.0f, 3.0f}, rotation_);
    } else {
      rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
      velocity_ = Rotate(glm::vec2{0.0f, 8.0f}, rotation_);
    }
  }
  bool should_die = false;
  if (tracking_duration_) {
    tracking_duration_--;
  } else {
    should_die = true;
  }
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == player_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 50.0f);
      should_die = true;
    }
  }
  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}
Tracking::~Tracking() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
