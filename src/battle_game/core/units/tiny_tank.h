#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Tank : public Unit {
 public:
  Tank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  // a list of glm::vec2 verteices for collision detection
  [[nodiscard]] std::vector<glm::vec2> GetVertices() const override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void DetectCollision();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  bool is_collision_{false};
  uint32_t fire_count_down_{0};
};
}  // namespace battle_game::unit
