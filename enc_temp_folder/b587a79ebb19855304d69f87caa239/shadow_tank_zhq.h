#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class ShadowTank : public Tank {
 public:
  ShadowTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  ;
  void Call_Shadow();
  void Call_Shadow_Click();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t shadow_count_down{360};
};
}  // namespace battle_game::unit
