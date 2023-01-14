#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Snowball_Sample_Sledge : public Tank {
 public:
  Snowball_Sample_Sledge(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void Fire();
  void TankMove(float move_speed, float rotate_angular_speed);
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  uint32_t fire_count_down_{0};
};
}  // namespace battle_game::unit