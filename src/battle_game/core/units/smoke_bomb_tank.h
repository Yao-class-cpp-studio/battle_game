#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class SmokeBombTank : public Tank {
 public:
  SmokeBombTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Update() override;
  void RenderHelper() override;

 protected:
  bool AimAt(const glm::vec2 &position);
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  glm::vec2 target_;
  bool aimed_;
  float max_distance_, radius_;
};
}  // namespace battle_game::unit
