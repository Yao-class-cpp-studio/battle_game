#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class QuicksilverTank : public Tank {
 public:
  QuicksilverTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Move(float move_speed, float rotate_angular_speed);
  void Fire();
  void Quicksilver();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t fire_count_down_{0};
  bool is_accelerate_{false};
  int accelerate_time_{0};
  int accelerate_count_down_{0};
};
}  // namespace battle_game::unit
