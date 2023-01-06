#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/jojo.h"

namespace battle_game::unit {
class TwoWayJet : public JoJo{
 public:
  TwoWayJet(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  void ShootFowardandBackward();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  bool is_second_scatter_{false};
};
}  // namespace battle_game::unit