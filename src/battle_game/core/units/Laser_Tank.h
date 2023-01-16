#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"
namespace battle_game::unit {
class Laser_Tank : public Tank {
 public:
  Laser_Tank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Destroy();
  void DestroyClick();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  bool iscombo{false};
  bool is_stuck_by_swamp{false};
  uint32_t destroy_count_down_{600};
  uint32_t isDestroy = 0;
};
}  // namespace battle_game::unit
