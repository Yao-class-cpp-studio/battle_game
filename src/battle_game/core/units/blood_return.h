#pragma once
#include "battle_game/core/object.h"
#include "battle_game/core/unit.h"
#include "glm/glm.hpp"

namespace battle_game::unit {
class BloodReturn : public Unit {
 public:
  BloodReturn(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;
  void Render() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  void RenderLifeBar() override;

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

 private:
  uint32_t count_down_{0};
  glm::vec2 scale_{0.4f};
};
}  // namespace battle_game::unit
