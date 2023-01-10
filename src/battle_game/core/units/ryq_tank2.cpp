#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/ryq_tank2.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

RYQ_Tank2::RYQ_Tank2(GameCore *game_core,
                                     uint32_t id,
                                     uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill skill_E;
  skill_E.type = E;
  skill_E.name = "Flash";
  skill_E.description = "Instantly move a distance.";
  skill_E.time_remain = 0;
  skill_E.time_total = kTickPerSecond * 15;
  skill_E.function = SKILL_ADD_FUNCTION(RYQ_Tank2::FlashClick);
  skills_.push_back(skill_E);
}

void RYQ_Tank2::Render() {
  Tank::Render();
}

void RYQ_Tank2::Update() {
  TankMove(3.0f, glm::radians(90.0f));//减小了旋转速度.
  TurretRotate();
  Flash();
  Fire();
}

void RYQ_Tank2::FlashClick() {
  glm::vec2 offset{0.0f};
  offset.y += 150.0f;
  float speed = 3.0f * GetSpeedScale();
  offset *= kSecondPerTick * speed;
  auto new_position =
      position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                        glm::vec3{0.0f, 0.0f, 1.0f}) *
                            glm::vec4{offset, 0.0f, 0.0f}};
  if (!game_core_->IsBlockedByObstacles(new_position)) {
    game_core_->PushEventMoveUnit(id_, new_position);
    flash_count_count_down_ = 15 * kTickPerSecond;
  }
}

void RYQ_Tank2::Flash() {
  skills_[0].time_remain = flash_count_count_down_;
  if (flash_count_count_down_) {
    flash_count_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E])
        FlashClick();
        
    }
  }

  
}

void RYQ_Tank2::Scatter() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    for (int i = -3; i <= 3; ++i) {
      auto offset = glm::radians(3.0f * i);
      velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
      GenerateBullet<bullet::CannonBall>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
          turret_rotation_ + offset, GetDamageScale(), velocity);
    }
  }
}

void RYQ_Tank2::Fire() {
  // check whether to perform second scatter
  if (second_scatter_count_down_) {
    second_scatter_count_down_--;
  } else {
    if (is_second_scatter_) {
      auto player = game_core_->GetPlayer(player_id_);
      if (player) {
        Scatter();
        is_second_scatter_ = false;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        Scatter();
        // wait for 0.5 second
        second_scatter_count_down_ = kTickPerSecond / 4;
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        is_second_scatter_ = true;
      }
    }
  }
}

const char *RYQ_Tank2::UnitName() const {
  return "RYQ_Tank2";
}

const char *RYQ_Tank2::Author() const {
  return "RYQ";
}
}  // namespace battle_game::unit
