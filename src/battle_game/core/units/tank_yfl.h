#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"
namespace battle_game::unit {
class TankYfl : public Tank {
 public:
  TankYfl(GameCore *game_core, uint32_t id, uint32_t player_id);

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  void SpeedUpClick();
  void SpeedUp();
  void Update() override;
  [[nodiscard]] float GetSpeedScale() const override;
  uint32_t IsSpeed = 0;
  uint32_t speedup_count_down = 0;
};
}  // namespace battle_game::unit
