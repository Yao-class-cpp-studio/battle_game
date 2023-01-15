#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Pentagon : public Unit {
 public:
  Pentagon(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void PentaMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void FFFire();
  void WeakDefense();
  void StrongDefense();
  void FFFireClick();
  void WeakDefenseClick();
  void StrongDefenseClick();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t fffire_count_down_{300};
  uint32_t weak_defense_count_down_{600};
  uint32_t strong_defense_count_down_{1200};
  uint32_t is_fffire_{0};
};
}  // namespace battle_game::unit
