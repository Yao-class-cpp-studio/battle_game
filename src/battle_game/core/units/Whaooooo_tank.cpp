#include "battle_game/core/units/Whaooooo_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

WhaoooooTank::WhaoooooTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void WhaoooooTank::Render() {
  Tank::Render();
}

void WhaoooooTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void WhaoooooTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::StunningBullet>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool WhaoooooTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *WhaoooooTank::UnitName() const {
  return "Whaooooo Tank";
}

const char *WhaoooooTank::Author() const {
  return "Whaooooo";
}
}  // namespace battle_game::unit