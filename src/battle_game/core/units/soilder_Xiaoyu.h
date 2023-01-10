#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class soilder : public Unit {
 public:
  soilder(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void soilderMove(float move_speed, float rotate_angular_speed);
  void bulletRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float bullet_rotation_{0.0f};
  uint32_t fire_count_down_{0};
};
}  // namespace battle_game::unit
