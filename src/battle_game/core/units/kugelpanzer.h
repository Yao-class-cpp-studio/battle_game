#pragma once
#include "battle_game/core/unit.h"
#include <queue>
namespace battle_game::unit {
class Kugelpanzer : public Unit {
 public:
  Kugelpanzer(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void Click();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t displacement_count_down_{0};
  double damping;
  std::queue<glm::vec2> displacement;
};
}  // namespace battle_game::unit
