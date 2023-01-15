#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class RushTank : public Tank {
 public:
  RushTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed) override;
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  void Rush();
  void RushClick();
  int time_stamp;
  int rush_count_down_;
};
}  // namespace battle_game::unit
