// cite: partially taken from "tiny_tank.h"
#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class CovidTank : public Unit {
 public:
  CovidTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] float GetBulletGravity() const;

 protected:
  void TankMove(
      float move_speed,
      float rotate_angular_speed);  // use W/S/A/D to operate
                                    // up/down/left/right, respectively.
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  float fire_speed_{10.0f};  // number of fires per second
  float bullet_velocicy_{20.0f};
  float bullet_gravity_{100.0f};
  int half_num_fork_bullet_{
      3};  // the number of forks of bullet is ( 2 * half_num_fork_bullet + 1 )
           // which is distributed uniformly
  float diff_angle_fork_bullet_{
      glm::radians(10.0f)};  // the difference in angle of forks of bullet
  bool is_fire_{false};      // shoot continuously
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
};
}  // namespace battle_game::unit
