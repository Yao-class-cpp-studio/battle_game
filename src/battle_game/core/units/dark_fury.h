#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class DarkFury : public Tank {
 public:
  DarkFury(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void Fire();
  void SwitchMode();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t fire_count_down_{0};
  uint32_t switch_mode_count_down_{0};
  bool is_berserk_{false};
};
}  // namespace battle_game::unit
