#pragma once
#include "battle_game/core/unit.h"
#include "tiny_tank.h"

namespace battle_game::unit {
class TankA : public Tank {
 public:
  TankA(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void Angel();
  void AngelClick();

  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t angel_count_down_{0};
};
}  // namespace battle_game::unit
