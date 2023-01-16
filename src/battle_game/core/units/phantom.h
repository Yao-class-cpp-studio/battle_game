#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/inferno_tank.h"

namespace battle_game::unit {
class Phantom : public InfernoTank {
 public:
  Phantom(GameCore *game_core, uint32_t id, uint32_t player_id);
  int GetRemainDodgeTime() const;
  void SetRemainDodgeTime(int new_remain_dodge_time);
  void Render() override;

 protected:
  int remain_dodge_time_{5};
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
   // add something to rerun
   // add something to teach QHT to generate 2PRs
