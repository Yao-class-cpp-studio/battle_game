#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Syl_tank : public Tank {
 public:
  Syl_tank(GameCore *game_core, uint32_t id, uint32_t player_id);
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
  uint32_t ThePowerFromAbyss_count_down_{600};
  uint32_t TheSalvationFromStars_count_down_{900};
  uint32_t isBurning{0};
  bool isBlessed{false};
};
}  // namespace battle_game::unit
