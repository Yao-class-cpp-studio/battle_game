#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Cart : public Unit {
 public:
  Cart(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void CartMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  glm::vec2 target_{glm::vec2{0.0f,0.0f}};
  float distance_{0.0f};
  float backforce_direction_{0.0f};
};
}  // namespace battle_game::unit
