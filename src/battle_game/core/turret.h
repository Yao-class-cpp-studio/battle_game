#pragma once
#include "battle_game/core/unit.h"
// #include "battle_game/core/obstacles/obstacles.h"

namespace battle_game::unit {
class NeutralTurret : public Unit {
 public:
  NeutralTurret(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

 private:
  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  float range_{5.0f};
  glm::vec2 scale_{1.0f};
  glm::vec2 findEnermy();
  bool setBlock_ = false;
};
}  // namespace battle_game::unit
