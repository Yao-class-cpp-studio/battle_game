#include "tangtang_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

tangtang_Tank::tangtang_Tank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void tangtang_Tank::Render() {
  Tank::Render();
}

void tangtang_Tank::Update() {
  TankMove(3.8f, glm::radians(180.0f));
  TurretRotate();
  tangtang_Fire();
}

void tangtang_Tank::tangtang_Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_number_++;
        if (fire_number_ % 3 != 0) {
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        } else {
          fire_count_down_ = kTickPerSecond / 10;
        }
      }
    }
  }
}

bool tangtang_Tank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *tangtang_Tank::UnitName() const {
  return "tangtang Tank";
}

const char *tangtang_Tank::Author() const {
  return "tangtang";
}
}  // namespace battle_game::unit
