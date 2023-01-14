#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include <ctime>

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

void RushTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  Rush();
  Fire();
}

RushTank::RushTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  time_stamp = -100000000;
  rush_count_down = 8 * kTickPerSecond; 
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
  time_stamp = clock();
  rush_count_down = 480;
}

void RushTank::Rush() {
  skills_[0].time_remain = rush_count_down;
  if (rush_count_down) {
    rush_count_down--;
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
  if( clock() - time_stamp <= 3000 ) {
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
