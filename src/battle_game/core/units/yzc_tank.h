#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Tankyzc : public Tank {
 public:
  Tankyzc(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;
  [[nodiscard]] float BasicMaxHealth() const override;
 protected:
  void Machinegun();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit