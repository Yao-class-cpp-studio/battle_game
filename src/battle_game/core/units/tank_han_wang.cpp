#include "battle_game/core/units/tank_han_wang.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"
// Format.

namespace battle_game::unit {

TankHanWang::TankHanWang(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void TankHanWang::Render() {
  Tank::Render();
}

void TankHanWang::Update() {
  TankMove(5.0f, glm::radians(360.0f));
  TurretRotate();
  Fire();
}

void TankHanWang::TankMove(float move_speed, float rotate_angular_speed) {
  Tank::TankMove(move_speed, rotate_angular_speed);
}

void TankHanWang::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Missile>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity, 24.0f);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

const char *TankHanWang::UnitName() const {
  return "Tank Han Wang";
}

const char *TankHanWang::Author() const {
  return "Han Wang";
}

}  // namespace battle_game::unit
