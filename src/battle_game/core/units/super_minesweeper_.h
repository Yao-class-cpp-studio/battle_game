#pragma once
#include "battle_game/core/unit.h"
namespace battle_game::unit {
class Super_minesweeper : public Unit {
 public:
  Super_minesweeper(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void ItemMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void Fireout(glm::vec2 velocity);
  bool is_teleport = false;
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  float turret_rotation_{0.0f};
  bool is_second = false;
  bool is_third = false;
  bool is_airborne = false;
  uint32_t fire_count_down_{0};
  uint32_t timer_{0};
  uint32_t timer2_{0};
  uint32_t timer_penetrate_{0};
  uint32_t tracking_timer_{0};
  uint32_t teleport_timer_ = 0;
  uint32_t buffer_timer_ = 0;
  uint32_t teleport_buffer = 0;
  glm::vec2 airborne_target = {0.0f, 0.0f};
  uint32_t airborne_timer = 0;
  uint32_t airborne_buffer = 0;
  uint32_t airborne_control_buffer = 0;
  uint32_t landing = 0;
};
}  // namespace battle_game::unit
