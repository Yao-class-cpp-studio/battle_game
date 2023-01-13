#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class CollisionTank : public Unit {
 public:
  CollisionTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  void InitAlreadyHit();
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void AlreadyHit();
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
  std::map<uint32_t, bool> alreadyhit;
  float speed_{0.0f};                  // 速度，有向的
  const float acceleration_{4.0f};     // 加速度，速度较低时使用
  const float power_{18.0f};           // 功率，速度较高时使用
  float friction_acceleration_{2.5f};  // 摩擦，按Q制动大幅加大
};
}  // namespace battle_game::unit