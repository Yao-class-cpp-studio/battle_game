#pragma once
#include "battle_game/core/unit.h"
#include "shield.h"
#include "tiny_tank.h"

namespace battle_game::unit {
class DefenceTank : public Tank {
 public:
  DefenceTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  void ShieldClick();
  void SetShield();
  void TurretClick();
  void SetTurret();
  void TurretsHealthDecrease();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

 private:
  uint32_t shield_count_down_{300};
  uint32_t turret_count_down_{600};
  std::vector<uint32_t> my_turret_ids_{};
};
}  // namespace battle_game::unit
