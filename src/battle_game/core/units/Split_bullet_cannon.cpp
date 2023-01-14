//
// Created by 13552 on 2023/1/8.
//
#include "Split_bullet_cannon.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"
namespace battle_game::unit {

TNL::TNL(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void TNL::Render() {
  Tank::Render();
}

void TNL::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void TNL::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::split_bullet>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);

        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}
const char *TNL::UnitName() const {
  return "Split bullet cannon";
}

const char *TNL::Author() const {
  return "QhtIsNotAvailable";
}
}  // namespace battle_game::unit
