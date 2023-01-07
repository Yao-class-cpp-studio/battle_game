#include "battle_game/core/units/dark_fury.h"
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include <random>
namespace battle_game::unit {

DarkFury::DarkFury(GameCore *game_core,
                                     uint32_t id,
                                     uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void DarkFury::Render() {
  Tank::Render();
}

void DarkFury::Update() {
  SwitchMode();
  if (is_berserk) {
    game_core_->PushEventDealDamage(id_, id_, 0.1);
  }
  TankMove(is_berserk? 6.0f:3.0f , glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void DarkFury::SwitchMode() {
  if (switch_mode_count_down_) {
    switch_mode_count_down_--;
  } else {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_RIGHT]) {
        is_berserk=!is_berserk;
    }
  }
  switch_mode_count_down_=kTickPerSecond;
  }
}

void DarkFury::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        if(is_berserk){
          auto err = glm::radians(0.0);
          err = (0.5 - game_core_->RandomFloat()) * glm::radians(30.0);
          auto velocity =Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + err);
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
        }
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = is_berserk? kTickPerSecond/5:kTickPerSecond;
      }
    }
  }
}

const char *DarkFury::UnitName() const {
  return "Dark Fury";
}

const char *DarkFury::Author() const {
  return "horipse";
}
}  // namespace battle_game::unit