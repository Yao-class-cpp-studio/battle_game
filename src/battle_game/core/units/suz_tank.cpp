#include "suz_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

SuzTank::SuzTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void SuzTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  LaserFire();
}

void SuzTank::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (!GetOperateMode()) {
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
      rotation_offset *=
          kSecondPerTick * rotate_angular_speed * GetSpeedScale();
      game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
    } else {
      if (input_data.key_down[GLFW_KEY_UP]) {
        offset.y += 1.0f;
      }
      if (input_data.key_down[GLFW_KEY_DOWN]) {
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
      if (input_data.key_down[GLFW_KEY_LEFT]) {
        rotation_offset += 1.0f;
      }
      if (input_data.key_down[GLFW_KEY_RIGHT]) {
        rotation_offset -= 1.0f;
      }
      rotation_offset *=
          kSecondPerTick * rotate_angular_speed * GetSpeedScale();
      game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
    }
  }
}

void SuzTank::LaserFire() {
  if (!(fire_count_down_ >= 0 && fire_count_down_ <= 20)) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        if (fire_count_down_ == 0) {
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        } else {
          fire_count_down_--;
        }
      }
    }
  }
}

const char *SuzTank::UnitName() const {
  return "Laser Tank";
}

const char *SuzTank::Author() const {
  return "S.U.Z";
}
}  // namespace battle_game::unit
