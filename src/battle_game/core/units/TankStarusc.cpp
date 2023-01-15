#include "TankStarusc.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

#include <iostream>

namespace battle_game::unit {

const float pi=acos(-1.0);

TankStarusc::TankStarusc(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) { }

void TankStarusc::Render() {
  Tank::Render();
}

void TankStarusc::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

// next step : 上下左右移动 WSAD

void TankStarusc::Fire() {
  // 回旋子弹
  float theta = pi*2/kTickPerSecond*mine_count_;
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, theta);
  GenerateBullet<bullet::CannonBall>(
    position_ + Rotate({0.0f,1.2f}, theta),
    theta, GetDamageScale()/10, velocity);
  ++mine_count_;
  if(mine_count_ >= kTickPerSecond) mine_count_ = 0;
  
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        fire_count_down_ = kTickPerSecond/2;  // Fire interval 1 second.
        fire_theta_ = turret_rotation_;
        fire_count_ = 3;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }

  if(fire_count_ > 0){
    --fire_count_;
    velocity = Rotate(glm::vec2{0.0f, 20.0f}, fire_theta_);
    GenerateBullet<bullet::CannonBall>(
        position_ + Rotate({0.0f, 1.2f}, fire_theta_),
        fire_theta_, GetDamageScale(), velocity);
  }
}

const char *TankStarusc::UnitName() const {
  return "TankStarusc";
}

const char *TankStarusc::Author() const {
  return "Starusc";
}
}  // namespace battle_game::unit
