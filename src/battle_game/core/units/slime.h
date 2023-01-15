#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Slime : public Unit {
 public:
  Slime(GameCore *game_core, uint32_t id, uint32_t player_id);
  ~Slime() override;
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] float BasicMaxHealth() const;

 protected:
  void SlimeMove(float move_speed, float rotate_angular_speed);
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t fire_count_down_{0};
  uint32_t death_split_{4};
};
}  // namespace battle_game::unit
