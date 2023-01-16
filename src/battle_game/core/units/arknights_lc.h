#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Arknights : public Tank {
 public:
  Arknights(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  float GetSpeedScale() const override;
  float GetHealthScale() const override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void ArknightsMove(float move_speed);
  void TurretRotate();
  void Fire();
  void Operators();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t operator_count_down_{300};
};
}  // namespace battle_game::unit
