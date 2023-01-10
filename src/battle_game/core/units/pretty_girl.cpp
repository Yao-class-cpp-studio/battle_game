#include "pretty_girl.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

PrettyGirl::PrettyGirl(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "SwitchStatus";
  temp.description = "Switch bullets between healing love and malicious blade";
  temp.time_remain = 0;
  temp.time_total = 60;
  temp.type = Q;
  temp.function = SKILL_ADD_FUNCTION(PrettyGirl::SwitchStatus);
  skills_.push_back(temp);
}

void PrettyGirl::Render() {
  Tank::Render();
}

void PrettyGirl::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  StatusCheck();
  Fire();
}

void PrettyGirl::SwitchStatus() {
  is_charming_ = !is_charming_;
  switch_count_down_ = 1 * kTickPerSecond;
}

void PrettyGirl::StatusCheck() {
  skills_[0].time_remain = switch_count_down_;
  if (switch_count_down_) {
    switch_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        SwitchStatus();
      }
    }
  }
}

void PrettyGirl::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] &&
          game_core_->GetUnits().size() > 1) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        if (is_charming_) {
          GenerateBullet<bullet::Rocket>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, -1., velocity);
        } else {
          GenerateBullet<bullet::Rocket>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, 1., velocity);
        }

        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

const char *PrettyGirl::UnitName() const {
  return "Pretty Girl";
}

const char *PrettyGirl::Author() const {
  return "Xiaoyang Ma";
}
}  // namespace battle_game::unit