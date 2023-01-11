#include "random_tank.h"

#include <cstdlib>
#include <ctime>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

RandomTank::RandomTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void RandomTank::Render() {
  Tank::Render();
}

void RandomTank::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    bool fx = 0;
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
      fx = 1;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
      fx = 1;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (fx) {
      glm::vec2 offset2{0.0f};
      offset2.y += (game_core_->RandomFloat() - 0.5);
      offset2 *= kSecondPerTick * speed;
      new_position = new_position +
                     glm::vec2{glm::rotate(glm::mat4{1.0f},
                                           (float)(rotation_ + acos(-1) / 2),
                                           glm::vec3{0.0f, 0.0f, 1.0f}) *
                               glm::vec4{offset2, 0.0f, 0.0f}};
    }
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

bool RandomTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}
void RandomTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}
void RandomTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity =
            Rotate(glm::vec2{0.0f, 20.0f},
                   turret_rotation_ + (game_core_->RandomFloat() - 0.5) * 0.7);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}
const char *RandomTank::UnitName() const {
  return "Random Tank";
}

const char *RandomTank::Author() const {
  return "Random";
}
}  // namespace battle_game::unit
