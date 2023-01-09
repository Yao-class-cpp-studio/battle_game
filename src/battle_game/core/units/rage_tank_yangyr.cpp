#include "rage_tank_yangyr.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
  RageTank::RageTank(GameCore * game_core, uint32_t id, uint32_t player_id)
      : Tank(game_core, id, player_id) {
  }

  void RageTank::Render() {
    Tank::Render();
  }

  void RageTank::Update() {
    TankMove(5.0f, glm::radians(180.0f));
    TurretRotate();
    RageFire();
  }

  void RageTank::RageFire() {
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
          firetimes_ += 1;
          if (firetimes_ % 3 == 0) {
            for (int i = 0 ； i < 4 ； i++) {
              GenerateBullet<bullet::CannonBall>(
                  position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                  turret_rotation_, GetDamageScale(), velocity);
            }
          } 
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        }
      }
    }
  }

  const char *RageTank::UnitName() const {
    return "Rage Tank";
  }

  const char *RageTank::Author() const {
    return "YYR";
  }
}  // namespace battle_game::unit
