#include "quicksilver.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
QuicksilverTank::QuicksilverTank(GameCore *game_core,
                                 uint32_t id,
                                 uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Quicksilver";
  temp.description = "Sacrificing attacking ability for 5 time speed";
  temp.time_remain = 0;
  temp.time_total = 420;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(QuicksilverTank::Quicksilver);
  skills_.push_back(temp);
}

void QuicksilverTank::Render() {
  if (is_accelerate_ == false)
    Tank::Render();
  else {
    SetTransformation(position_, rotation_, glm::vec2{1.2f});
    SetColor(game_core_->GetPlayerColor(player_id_));
    SetTexture("../../textures/quicksilver.png");
    DrawModel(0);
  }
}

void QuicksilverTank::Update() {
  Move(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
  Quicksilver();
}

void QuicksilverTank::Move(float move_speed, float rotate_angular_speed) {
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
    if (is_accelerate_ == true) {
      speed = speed * 5;
      accelerate_time_ += 1;
    }
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
    if (input_data.key_down[GLFW_KEY_E] && accelerate_count_down_ == 0) {
      is_accelerate_ = true;
      accelerate_time_ = 0;
    }
    if (accelerate_time_ == 240)
      is_accelerate_ = false;
  }
}

void QuicksilverTank::Fire() {
  if (is_accelerate_ == false)
    Tank::Fire();
}

void QuicksilverTank::Quicksilver() {
  skills_[0].time_remain = accelerate_count_down_;
  if (accelerate_count_down_) {
    accelerate_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        accelerate_count_down_ = 420;
      }
    }
  }
}

bool QuicksilverTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *QuicksilverTank::UnitName() const {
  return "Quicksilver";
}

const char *QuicksilverTank::Author() const {
  return "YYR";
}
}  // namespace battle_game::unit
