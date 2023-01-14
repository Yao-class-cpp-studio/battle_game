#pragma once
#include "tiny_tank.h"

namespace battle_game::unit {
class ReboundingSampleTank : public Tank {
 public:
  ReboundingSampleTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  uint32_t rebound_times_{2};
};
}  // namespace battle_game::unit
