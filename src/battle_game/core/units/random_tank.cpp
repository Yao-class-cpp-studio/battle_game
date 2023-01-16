#include "battle_game/core/units/random_tank.h"

#include <cstdlib>

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

void RandomTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  RandomFire();
}

void RandomTank::RandomFire() {
  if (random_number_ > 10) {
    random_number_ = (rand()) % 9 + 1;
  }
  if (fire_count_down_ < random_number_ * random_number_ ||
      fire_count_down_ % random_number_ != 0) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.5f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        if (fire_count_down_ == 0) {
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
          random_number_ = (rand()) % 9 + 1;
        } else {
          fire_count_down_--;
        }
      }
    }
  }
}

const char *RandomTank::UnitName() const {
  return "RandomTank";
}

const char *RandomTank::Author() const {
  return "ninemilewalk";
}
}  // namespace battle_game::unit
