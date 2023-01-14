#include "double_shot_tank.h"
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

DoubleShotTank::DoubleShotTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void DoubleShotTank::Render() {
  Tank::Render();
}

void DoubleShotTank::Update() {
  TankMove(7.0f, glm::radians(180.0f));
  TurretRotate();
  DoubleShot();
}

bool DoubleShotTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *DoubleShotTank::UnitName() const {
  return "Double Shot Tank";
}

const char *DoubleShotTank::Author() const {
  return "ZiXi Cai";
}

void DoubleShotTank::DoubleShot() {
  if (fire_count_down_ != 0 && fire_count_down_ != 1 && fire_count_down_ != 11)
    fire_count_down_--;
  else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] || fire_count_down_ == 1) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        if (fire_count_down_ == 11 || fire_count_down_ == 0)
          fire_count_down_ = 10;
        else 
          fire_count_down_ = kTickPerSecond;
      }
    }
  }
}

}  // namespace battle_game::unit
