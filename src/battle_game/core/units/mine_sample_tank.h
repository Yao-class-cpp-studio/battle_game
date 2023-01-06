#pragma once
#include "battle_game/core/unit.h"
#include "tiny_tank.h"

namespace battle_game::unit {
class MineSampleTank : public Tank {
 public:
  MineSampleTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void PutMine();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
