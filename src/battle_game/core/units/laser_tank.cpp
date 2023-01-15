#include "laser_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

LaserTank::LaserTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void LaserTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void LaserTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Laser>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

const char *LaserTank::UnitName() const {
  return "Laser Tank";
}

const char *LaserTank::Author() const {
  return "pangker7";
}
}  // namespace battle_game::unit