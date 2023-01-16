#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class AllRound : public Tank {
 public:
  AllRound(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void Fire();
  void SwitchMode1();
  void SwitchMode2();
  void SwitchMode3();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t fire_count_down_{0};
  uint32_t switch_mode_count_down_{0};
  int mode_ = 1;
};
}  // namespace battle_game::unit
