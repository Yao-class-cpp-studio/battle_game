#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class AllRoundTank : public Tank {
 public:
  AllRoundTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  [[nodiscard]] float GetDamageScale() const;
  [[nodiscard]] float GetSpeedScale() const;

  void SpeedMode();
  void DamageMode();
  void RecoveryMode();

  void SwitchMode();

  float speed_scale_{1.4f};
  float damage_scale_{1.0f};
  uint32_t current_mode_{0};
};
}  // namespace battle_game::unit
