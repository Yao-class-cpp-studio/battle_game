#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/heavy_tank.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

HeavyTank::HeavyTank(GameCore *game_core,
                                     uint32_t id,
                                     uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void HeavyTank::Render() {
  Tank::Render();
}

void HeavyTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void HeavyTank::Fire() {
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
        glm::vec2 offset{0.0f};
        offset.y -= 1.0f;
        float speed =4.0* GetSpeedScale();
        offset *= kSecondPerTick * speed;
        auto new_position =
            position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, turret_rotation_,
                                              glm::vec3{0.0f, 0.0f, 1.0f}) *
                                  glm::vec4{offset, 0.0f, 0.0f}};

        if (!game_core_->IsBlockedByObstacles(new_position)) {
          game_core_->PushEventMoveUnit(id_, new_position);
        }

        fire_count_down_ = kTickPerSecond*2;  // Fire interval 1 second.
      }
    }
  }
}

const char *HeavyTank::UnitName() const {
  return "Heavy Tank";
}

const char *HeavyTank::Author() const {
  return "skydogli";
}
}  // namespace battle_game::unit
