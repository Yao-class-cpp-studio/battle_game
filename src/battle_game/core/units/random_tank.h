#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class RandomTank : public Tank {
  RandomTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void RandomFire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

 protected:
  uint32_t random_number_{2};
};
}  // namespace battle_game::unit