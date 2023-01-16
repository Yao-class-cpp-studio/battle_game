#include "battle_game/core/units/all_round_tank.h"

#include <random>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
namespace battle_game::unit {

AllRound::AllRound(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill mode1("speed mode", Q);
  mode1.function = SKILL_ADD_FUNCTION(AllRound::SwitchMode);
  mode1.time_remain = 0;
  mode1.time_total = 120;
  skills_.push_back(mode1);

  Skill mode2("damage mode", E);
  mode2.function = SKILL_ADD_FUNCTION(AllRound::SwitchMode);
  mode2.time_remain = 0;
  mode2.time_total = 120;
  skills_.push_back(mode2);

  Skill mode3("health mode", R);
  mode3.function = SKILL_ADD_FUNCTION(AllRound::SwitchMode);
  mode3.time_remain = 0;
  mode3.time_total = 120;
  skills_.push_back(mode3);
}

void AllRound::Render() {
  Tank::Render();
}

void AllRound::Update() {
  SwitchMode();
  if (mode_ == 1) {
    TankMove(6.0f, glm::radians(180.0f));
  } else {
    TankMove(3.0f, glm::radians(180.0f));
  }
  if (mode_ == 3) {
    if (health_ <= 0.999) {
      health_ += 0.001;
    }
  }
  TurretRotate();
  Fire();
}

void AllRound::SwitchMode() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.key_down[GLFW_KEY_Q]) {
      mode_ = 1;
    }
    if (input_data.key_down[GLFW_KEY_E]) {
      mode_ = 2;
    }
    if (input_data.key_down[GLFW_KEY_R]) {
      mode_ = 3;
    }
  }
}

void AllRound::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        if (mode_ == 2) {
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale() * 2, velocity);
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        } else {
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        }
      }
    }
  }
}

const char *AllRound::UnitName() const {
  return "All-Round tank";
}

const char *AllRound::Author() const {
  return "xyiiiiiii";
}
}  // namespace battle_game::unit
