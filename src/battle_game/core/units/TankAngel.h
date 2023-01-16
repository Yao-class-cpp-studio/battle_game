#pragma once
#include "battle_game/core/unit.h"
#include "tiny_tank.h"

namespace battle_game::unit {
class TankAngel : public Tank {
 public:
  TankAngel(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  float BasicMaxHealth() const override;

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
