#include "battle_game/core/units/split_bullet_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"

namespace battle_game::unit {

SplitBulletTank::SplitBulletTank(GameCore *game_core,
                                 uint32_t id,
                                 uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void SplitBulletTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void SplitBulletTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 5.0f}, turret_rotation_);
        GenerateBullet<bullet::SplitBullet>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

const char *SplitBulletTank::UnitName() const {
  return "Split Bullet Tank";
}

const char *SplitBulletTank::Author() const {
  return "pummmmpkin";
}
}  // namespace battle_game::unit
