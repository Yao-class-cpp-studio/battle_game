#include "triple_shot_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

TripleShotTank::TripleShotTank(GameCore *game_core,
                               uint32_t id,
                               uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void TripleShotTank::Render() {
  Tank::Render();
}

void TripleShotTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  TripleFire();
}

void TripleShotTank::TripleFire() {
  if (fire_count_down_ != 0 && fire_count_down_ != 3 && fire_count_down_ != 6) {
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
        } else {
          fire_count_down_--;
        }
      }
    }
  }
}

const char *TripleShotTank::UnitName() const {
  return "Triple Shot Tank";
}

const char *TripleShotTank::Author() const {
  return "Weihang Xu";
}
}  // namespace battle_game::unit
