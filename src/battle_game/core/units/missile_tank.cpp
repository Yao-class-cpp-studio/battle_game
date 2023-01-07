#include "missile_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

MissileTank::MissileTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void MissileTank::Render() {
  Tank::Render();
}

void MissileTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

bool MissileTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

void MissileTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 18.0f}, turret_rotation_);
        GenerateBullet<bullet::Missile>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity, 24.0f);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

const char *MissileTank::UnitName() const {
  return "Missile Tank";
}

const char *MissileTank::Author() const {
  return "unprintable123";
}
}  // namespace battle_game::unit
