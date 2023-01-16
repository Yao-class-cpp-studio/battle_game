#include <ctime>

#include "battle_game/core/game_core.h"

namespace battle_game::unit {

void RushTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  Rush();
  Fire();
}

RushTank::RushTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  rush_count_down_ = 480;
  time_stamp_ = -100000000;
  Skill temp;
  temp.time_total = 500;
  temp.time_remain = 0;
  temp.name = "Rush";
  temp.description = "Move Faster!";
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(RushTank::RushClick);
  skills_.push_back(temp);
}

void RushTank::RushClick() {
  rush_count_down_ = 480;
  time_stamp_ = clock();
}

void RushTank::Rush() {
  skills_[0].time_remain = rush_count_down_;
  if (rush_count_down_) {
    rush_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        RushClick();
      }
    }
  }
}

void RushTank::TankMove(float move_speed, float rotate_angular_speed) {
  if (clock() - time_stamp_ <= 3000) {
    move_speed *= 5;
  }
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

const char *RushTank::UnitName() const {
  return "Rush Tank";
}

const char *RushTank::Author() const {
  return "zhqwq";
}
}  // namespace battle_game::unit
