#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class QHCritTank : public Tank {
 public:
  QHCritTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void CritFire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  float bullet_velocity_{10.0f};
};
}  // namespace battle_game::unit
