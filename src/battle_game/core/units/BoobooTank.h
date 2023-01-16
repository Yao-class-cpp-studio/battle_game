#pragma once
#include "battle_game/core/units/tiny_tank.h"
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class BoobooTank : public Tank {
 public:
  BoobooTank(GameCore *game_core, uint32_t id, uint32_t player_id);

 protected:
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
