#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class ZibengDog : public Unit {
 public:
  ZibengDog(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void DogMove(float move_speed, float rotate_angular_speed);
  void MuzzleRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float muzzle_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t zibeng_line_time_{0};
  bool should_beng_{false};
};
}  // namespace battle_game::unit
