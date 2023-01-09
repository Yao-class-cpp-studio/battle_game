#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class SplitBulletTank : public Tank {
 public:
  SplitBulletTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void Fire();

  uint32_t fire_count_down_{0};
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
