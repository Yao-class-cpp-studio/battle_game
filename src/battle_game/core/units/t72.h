#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class T72 : public Unit {
 public:
  T72(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  float GetSpeedScale() const override;
  float GetHealthScale() const override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void GunMove(float move_speed, float rotate_angular_speed);
  void DirectionRotate();
  void Fire();
  void Thunderbolt();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float direction_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t thunderbolt_count_down_{0};
  uint32_t charging_time_{180};
  bool should_shoot_{false};
};
}  // namespace battle_game::unit
