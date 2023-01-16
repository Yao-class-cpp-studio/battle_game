#pragma once
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Accumulator : public Tank {
 public:
  Accumulator(GameCore *game_core, uint32_t id, uint32_t player_id);
  void RenderHelper();
  void Update() override;

 protected:
  void Fire();
  
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float damage_scale_{0};
  const float accumulate_per_tick;
};
}  // namespace battle_game::unit
