#include "boomerang_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

BoomerangTank::BoomerangTank(GameCore *game_core,
                             uint32_t id,
                             uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void BoomerangTank::Render() {
  Tank::Render();
}

void BoomerangTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void BoomerangTank::Fire() {
  if (fire_count_down_ != 0) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Boomerang>(
            position_ + Rotate({0.0f, 1.5f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;
      }
    }
  }
}

const char *BoomerangTank::UnitName() const {
  return "Boomerang Tank";
}

const char *BoomerangTank::Author() const {
  return "YYR";
}
}  // namespace battle_game::unit
