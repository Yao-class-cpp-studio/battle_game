#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class QXTank1 : public Tank {
 public:
  QXTank1(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;
  // 1
 protected:
  void CritFire();

  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
