#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "circle_test_tank.h"
#include "tiny_tank.h"

namespace battle_game::unit {

CircleTestTank::CircleTestTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void CircleTestTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void CircleTestTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        GenerateBullet<bullet::Circle>(
            position_, turret_rotation_, GetDamageScale(), 5);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

const char *CircleTestTank::UnitName() const {
  return "Test Tank For Circle";
}

const char *CircleTestTank::Author() const {
  return "tzs";
}
}  // namespace battle_game::unit
