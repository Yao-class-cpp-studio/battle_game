#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class ShadowTank : public Tank {
 public:
  ShadowTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void CallShadow();
  void CallShadowClick();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t shadow_count_down_{1800};
};
}  // namespace battle_game::unit
