#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Sparky : public Tank {
 public:
  Sparky(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render();
  void Update();
  float GetDamageScale() const;
  float GetHealthScale() const;
  float GetSpeedScale() const override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
};
}  // namespace battle_game::unit
