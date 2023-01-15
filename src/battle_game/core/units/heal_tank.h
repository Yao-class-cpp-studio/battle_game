#pragma once

#include "battle_game/core/game_core.h"
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class HealTank : public Tank {
 public:
  HealTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] float GetDamageScale() const override;
  [[nodiscard]] float GetHealthScale() const override;
  [[nodiscard]] float BasicMaxHealth() const override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  bool IsInhale() const;

 protected:
  void Fire();
  void Heal();
  void Expand();
  void InhaleClick();
  void RefreshClick();
  void Inhale();
  void Refresh();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t fire_count_down_{0};
  uint32_t inhale_count_down_{900};
  uint32_t isinhale_count_down_{0};
  uint32_t refresh_count_down_{2700};
  uint32_t heal_count_down_{5};
  uint32_t expand_count_down_{900};
  float health_scale_{1.0f};
};
}  // namespace battle_game::unit