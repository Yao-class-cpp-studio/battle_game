#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class RandomTank : public Tank {
 public:
  RandomTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;

  void RandomFire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  uint32_t random_number_ = 0;
};
}  // namespace battle_game::unit