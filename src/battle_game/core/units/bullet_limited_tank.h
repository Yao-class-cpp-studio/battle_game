#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class BulletLimitedTank : public Tank {
 public:
  BulletLimitedTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  void Reload();
  void ReloadClick();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t second_scatter_count_down_{0};
  bool is_second_scatter_{false};
  const uint32_t bullet_limit_{10};
  uint32_t bullet_remaining_{bullet_limit_};
  uint32_t reload_count_down_{0};
};
}  // namespace battle_game::unit
