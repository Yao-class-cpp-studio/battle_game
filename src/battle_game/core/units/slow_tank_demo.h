#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class SlowTank : public Tank {
 public:
  SlowTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void Fire();
  void TurretRotate();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit

