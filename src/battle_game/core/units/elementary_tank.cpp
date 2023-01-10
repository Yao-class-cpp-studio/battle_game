#include "elementary_tank.h"

#include <ctime>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"

namespace battle_game::unit {

ElementaryTank::ElementaryTank(GameCore *game_core,
                               uint32_t id,
                               uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill tmp;
  tmp.name = "Accelerate";
  tmp.time_remain = 0;
  tmp.time_total = 700;
  tmp.type = Q;
  tmp.function = SKILL_ADD_FUNCTION(ElementaryTank::Accelerate);
  skills_.push_back(tmp);
  tmp.name = "Rage Shooting";
  tmp.time_remain = 0;
  tmp.time_total = 800;
  tmp.type = E;
  tmp.function = SKILL_ADD_FUNCTION(ElementaryTank::RageShoot);
  skills_.push_back(tmp);
}

void ElementaryTank::Render() {
  Tank::Render();
};

void ElementaryTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Accelerate();
  RageShoot();
  Fire();
}

void ElementaryTank::TankMove(float move_speed, float rotate_angular_speed) {
  if (accelerate_flag_)
    Tank::TankMove(move_speed * 2, rotate_angular_speed);
  else
    Tank::TankMove(move_speed, rotate_angular_speed);
}

void ElementaryTank::Accelerate() {
  if (accelerate_time_ <= 360)
    accelerate_time_ += 1;
  else
    accelerate_flag_ = false;
  if (skills_[0].time_remain) {
    skills_[0].time_remain -= 1;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        accelerate_flag_ = true;
        accelerate_time_ = 0;
        skills_[0].time_remain = skills_[0].time_total;
        num_strong_bullets_ = 3;
      }
    }
  }
}

void ElementaryTank::RageShoot() {
  if (rageshoot_time_ <= 400)
    rageshoot_time_ += 1;
  else
    rageshoot_flag_ = false;
  if (skills_[1].time_remain) {
    skills_[1].time_remain -= 1;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        rageshoot_flag_ = true;
        rageshoot_time_ = 0;
        skills_[1].time_remain = skills_[1].time_total;
        for (int i = -10; i <= 10; ++i) {
          auto offset = glm::radians(7.0f * i);
          auto velocity =
              Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
              turret_rotation_ + offset, GetDamageScale(), velocity);
        }
      }
    }
  }
}

void ElementaryTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        if (num_strong_bullets_) {
          GenerateBullet<bullet::Rocket>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
          num_strong_bullets_ -= 1;
          fire_count_down_ = kTickPerSecond * 0.5;
        } else if (rageshoot_flag_) {
          GenerateBullet<bullet::Shuriken>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity, true);
          fire_count_down_ = kTickPerSecond * 0.25;
        } else {
          GenerateBullet<bullet::Shuriken>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity, false);
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        }
      }
    }
  }
}

bool ElementaryTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *ElementaryTank::UnitName() const {
  return "Elementary Tank";
}

const char *ElementaryTank::Author() const {
  return "Wzrrr";
}
}  // namespace battle_game::unit
