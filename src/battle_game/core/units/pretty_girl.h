#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class PrettyGirl : public Tank {
 public:
  PrettyGirl(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void Fire();
  void StatusCheck();
  void SwitchStatus();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t switch_count_down_{0};
  bool is_charming_{false};
};
}  // namespace battle_game::unit
