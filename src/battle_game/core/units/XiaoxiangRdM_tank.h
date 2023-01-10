#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class XXRdMTank : public Unit {
 public:
  XXRdMTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  void Teleport();
  void TeleportClick(bool isforward);
  void TeleportFClick();
  void TeleportBClick();
  void Healthcare();
  void HealthcareClick();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t teleport_count_down_{360};
  uint32_t healthcare_count_down_{1200};

  float x_0_ = 0.8; //semi-minor axis
  float y_0_ = 1.2; //semi-major axis
};
}  // namespace battle_game::unit
