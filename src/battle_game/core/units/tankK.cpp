#include "battle_game/core/units/tankK.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

TankK::TankK(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void TankK::Render() {
  Tank::Render();
}

void TankK::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void TankK::Fire() {
  if (fire_count_down_ && fire_count_down_ != 3) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        for (int i = -2; i <= 2; ++i)
          if (i) {
            auto offset = glm::radians(45.0f * i);
            auto velocity =
                Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
            GenerateBullet<bullet::CannonBall>(
                position_ + Rotate({0.0f, 1.3f}, turret_rotation_),
                turret_rotation_ + offset, GetDamageScale(), velocity);
          }
        if (fire_count_down_)
          fire_count_down_--;
        else
          fire_count_down_ = kTickPerSecond / 2;  // Fire interval 1 second.
      }
    }
  }
}

bool TankK::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *TankK::UnitName() const {
  return "TankK";
}

const char *TankK::Author() const {
  return "QAQAutoMaton";
}
}  // namespace battle_game::unit
