#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class FallenAngel : public Tank {
 public:
  FallenAngel(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  void ThePowerFromAbyss();
  void ThePowerFromAbyssClick();
  void TheSalvationFromStars();
  void TheBlessOfGod();
  void TheSalvationFromStarsClick();
  [[nodiscard]] float GetDamageScale() const override;
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t the_power_from_abyss_count_down_{300};
  uint32_t the_salvation_from_stars_count_down_{900};
  uint32_t is_burning_{0};
  bool is_blessed_{false};
};
}  // namespace battle_game::unit
