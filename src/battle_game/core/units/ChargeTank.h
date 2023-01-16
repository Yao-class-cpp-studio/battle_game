#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class ChargeTank : public Tank {
 public:
  ChargeTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
  float start_;
  float end_; //qia biao ; count the time
  int count_{0};
  float count_down_;
};
}  // namespace battle_game::unit