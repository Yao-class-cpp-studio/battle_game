#include "battle_game/core/units/Random_tank.h"

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
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        int tms = game_core_->RandomInt(1, 5);
        for (int i = 0; i < tms; i++) {
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
          fire_count_down_ = 2 * kTickPerSecond;  // Fire interval 2 second.
        }
      }
    }
  }
}

bool RandomTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *RandomTank::UnitName() const {
  return "Random Tank";
}

const char *RandomTank::Author() const {
  return "JohnVictor36";
}
}  // namespace battle_game::unit