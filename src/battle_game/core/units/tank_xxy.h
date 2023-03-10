#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class TankXxy : public Tank {
 public:
  TankXxy(GameCore *game_core, uint32_t id, uint32_t player_id);

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
