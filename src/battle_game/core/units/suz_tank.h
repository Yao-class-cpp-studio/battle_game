#pragma once
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class SuzTank : public Tank {
 public:
  SuzTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void LaserFire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
