#include "qx_tank_1.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
// 1
QXTank1::QXTank1(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void QXTank1::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  CritFire();
}

void QXTank1::CritFire() {
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
        fire_count_down_ = kTickPerSecond;
      }
    }
  }
}

const char *QXTank1::UnitName() const {
  return "QXTank1";
}

const char *QXTank1::Author() const {
  return "QIXUAN";
}
}  // namespace battle_game::unit
