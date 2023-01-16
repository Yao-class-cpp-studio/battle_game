#include "TankStarusc.h"

#include <iostream>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

const float pi = acos(-1.0);

TankStarusc::TankStarusc(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill qwq;
  qwq.name = "Shield";
  qwq.description = "Can only attack from the front.";
  qwq.time_remain = 0;
  qwq.time_total = 12 * kTickPerSecond;
  qwq.type = E;
  qwq.function = SKILL_ADD_FUNCTION(TankStarusc::ShieldClick);
  skills_.push_back(qwq);
}

void TankStarusc::Render() {
  Tank::Render();
}

void TankStarusc::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Shield();
  Fire();
}

// next step : 上下左右移动 WSAD

void TankStarusc::ShieldClick() {
  have_shield_ = 10 * kTickPerSecond;
  shield_count_down_ = 12 * kTickPerSecond;
}

void TankStarusc::Shield() {
  skills_[0].time_remain = shield_count_down_;
  if (have_shield_)
    --have_shield_;
  if (shield_count_down_)
    --shield_count_down_;
  else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E])
        ShieldClick();
    }
  }
}

void TankStarusc::Fire() {
  // 回旋子弹
  float theta = pi * 2 / kTickPerSecond * mine_count_;
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, theta);
  GenerateBullet<bullet::CannonBall>(position_ + Rotate({0.0f, 1.2f}, theta),
                                     theta, GetDamageScale() / 10, velocity);
  ++mine_count_;
  if (mine_count_ >= kTickPerSecond)
    mine_count_ = 0;

  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        fire_count_down_ = kTickPerSecond / 2;  // Fire interval 1 second.
        fire_theta_ = turret_rotation_;
        fire_count_ = 3;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }

  if (fire_count_ > 0) {
    --fire_count_;
    velocity = Rotate(glm::vec2{0.0f, 20.0f}, fire_theta_);
    GenerateBullet<bullet::CannonBall>(
        position_ + Rotate({0.0f, 1.2f}, fire_theta_), fire_theta_,
        GetDamageScale(), velocity);
  }
}

float TankStarusc::Vec2Cross(glm::vec2 x, glm::vec2 y) const {
  return x.x * y.y - x.y * y.x;
}

bool TankStarusc::CheckDamage(glm::vec2 position, glm::vec2 velocity) const {
  // 0 -> damage
  if (!have_shield_)
    return 0;
  /*
  tank_body_model_index 坦克底座大小
  判断子弹所在射线与坦克前面的直线(x,y)交点是否位于线段(x,y)上
  */
  if (glm::dot(velocity, Rotate(glm::vec2{0.0f, 1.0f}, rotation_)) >
      eps_starusc_)
    return 1;
  position = WorldToLocal(position);
  glm::vec2 x = Rotate(glm::vec2{0.8f, 1.0f}, rotation_),
            y = Rotate(glm::vec2{-0.8f, 1.0f}, rotation_);
  float a = TankStarusc::Vec2Cross(y - x, position - x),
        b = TankStarusc::Vec2Cross(position + velocity - x, y - x);
  glm::vec2 intersection = position + velocity * (a / (a + b));
  if (glm::dot(x - intersection, y - intersection) < -eps_starusc_)
    return 0;
  return 1;
}

const char *TankStarusc::UnitName() const {
  return "TankStarusc";
}

const char *TankStarusc::Author() const {
  return "Starusc";
}
}  // namespace battle_game::unit
