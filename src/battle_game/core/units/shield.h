#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Shield : public Unit {
 public:
  Shield(GameCore *game_core,
         uint32_t id,
         uint32_t player_id,
         uint32_t protected_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void ShieldMove();
  void HealthDecrease();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

 private:
  uint32_t lord_id_{0};
  glm::vec2 scale_{2.0f};
};
}  // namespace battle_game::unit
