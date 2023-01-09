#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class RYQ_Tank : public Unit {
 public:
  RYQ_Tank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire1_count_down_1{0};//一武射击频率.
  uint32_t fire1_count_down_2{0};//一武冷却.
  uint32_t fire1_count_num_{120};   // 一武弹药数.
  uint32_t fire2_count_down_{0};   // 二武冷却.
  uint32_t fire2_count_num_{2};//二武弹药数.
  uint32_t skillE_count_down_{0};//E技能冷却.
  uint32_t skillQ_count_down_{0};//Q技能冷却.
};
}  // namespace battle_game::unit
