#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/swift_tank.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

SwiftTank::SwiftTank(GameCore *game_core,
                                     uint32_t id,
                                     uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void SwiftTank::Render() {
  Tank::Render();
}

void SwiftTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void SwiftTank::TankMove(float move_speed, float rotate_angular_speed) {
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
    if (input_data.key_down[GLFW_KEY_F]&&f_count_) {
      offset.y += 1.0f * kTickPerSecond;
      f_count_ = 0;
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
      rotation_offset += 2.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 2.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void SwiftTank::Fire() {
  if (running_count_down_<360 ) {
    running_count_down_++;
  }
  if (fire_count_down_) {
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
        if (running_count_down_ == 360) {
          for (int i = 0; i < 4; i++) {
            GenerateBullet<bullet::CannonBall>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, GetDamageScale(), velocity);
          }
          running_count_down_ = 0;
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool SwiftTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *SwiftTank::UnitName() const {
  return "Swift Tank";
}

const char *SwiftTank::Author() const {
  return "zq-chen22";
}
}  // namespace battle_game::unit
