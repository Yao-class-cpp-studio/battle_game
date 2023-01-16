#include "crit_tank_qxhwj.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

QHCritTank::QHCritTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void QHCritTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  CritFire();
}

void QHCritTank::CritFire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity =
            Rotate(glm::vec2{0.0f, bullet_velocity_}, turret_rotation_);
        GenerateBullet<bullet::RandomBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

const char *QHCritTank::UnitName() const {
  return "Crit Tank";
}

const char *QHCritTank::Author() const {
  return "QIXUAN and hwj";
}
}  // namespace battle_game::unit
