#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class RYQ_Tank2 : public Tank {
 public:
  RYQ_Tank2(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void Fire();
  void Scatter();
  void FlashClick();
  void Flash();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t second_scatter_count_down_{0};
  uint32_t flash_count_count_down_{60*15};
  bool is_second_scatter_{false};
};
}  // namespace battle_game::unit
