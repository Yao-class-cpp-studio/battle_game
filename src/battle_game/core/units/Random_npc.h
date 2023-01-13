#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class RandomNPC : public Tank {
 public:
  RandomNPC(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] float GetDamageScale() const override;
  [[nodiscard]] float BasicMaxHealth() const override;

 protected:
  void Fire();
  void RandomMove();
  void RandomTurretRotate();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  int current_steps;
  int current_direction;
  // In this implementation,the moving direction will change once a few steps,or
  // it will just wander around,which looks absurd.
};
}  // namespace battle_game::unit