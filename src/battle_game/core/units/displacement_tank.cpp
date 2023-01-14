#include "displacement_tank.h"
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

DisplacementTank::DisplacementTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void DisplacementTank::Render() {
  Tank::Render();
}

void DisplacementTank::Update() {
  TurretRotate();
  if(displacement_count == 7)
    Displacement();
  else
    Tribleshot();
}

bool DisplacementTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *DisplacementTank::UnitName() const {
  return "Displacement Tank";
}

const char *DisplacementTank::Author() const {
  return "ZiXi Cai";
}

void DisplacementTank::Tribleshot() {
  TankMove(3.0f, glm::radians(180.0f));
  if (fire_count_down_ != 0 && fire_count_down_ != 1 && 
      fire_count_down_ != 8 && fire_count_down_ != 15)
    fire_count_down_--;
  else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] || 
          fire_count_down_ == 1 || fire_count_down_ == 8) {
        displacement_count++;
        if(displacement_count != 7) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        if(fire_count_down_ == 0)
          fire_count_down_ = 15;
        if (fire_count_down_ == 8 || fire_count_down_ == 15)
          fire_count_down_--;
        else 
          fire_count_down_ = kTickPerSecond;
        }
      }
    }
  }
}

void DisplacementTank::Displacement() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if(fire_count_down_ == 14) {
      auto velocity = Rotate(glm::vec2{0.0f, 40.0f}, turret_rotation_);
      GenerateBullet<bullet::CannonBall>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
          turret_rotation_, GetDamageScale(), velocity);
    }
    fire_count_down_--;
    if(fire_count_down_ != 0)
      TankMove(20.0f, glm::radians(180.0f));
    else {
      fire_count_down_ = kTickPerSecond;
      displacement_count = 0;
      TankMove(3.0f, glm::radians(180.0f));
    }
  }
}

}  // namespace battle_game::unit