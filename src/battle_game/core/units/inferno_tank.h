#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class InfernoTank : public Tank {
 public:
  InfernoTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  void Inferno();
  void Hidden();
  void Block();
  void HiddenClick();
  void BlockClick();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t shoot_count_down_{240};
  uint32_t hidden_count_down_{600};
  uint32_t block_count_down_{600};
  uint32_t isHidden{0};
};
}  // namespace battle_game::unit
