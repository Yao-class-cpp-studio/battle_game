#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Lightening : public Unit {
 public:
  Lightening(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void LighteningMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  bool Flash();
  void FlashClick();

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  void RocketClick();
  void Rocket();
  uint32_t rocket_count_down_{0};
  uint32_t flash_duration_{0};

  uint32_t flash_count_down_{0};
};
}  // namespace battle_game::unit

// for test
