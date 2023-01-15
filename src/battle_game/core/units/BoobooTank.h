#pragma once
#include "battle_game/core/units/tiny_tank.h"
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class BoobooTank : public Tank {
 public:
  BoobooTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;

 protected:
  void Fire();
  glm::vec4 GetPlayerColor_booboo(uint32_t player_id, uint32_t render_perspective_) const {
    if (render_perspective_ == 0) {
      return glm::vec4{0.5f, 1.0f, 0.5f, 1.0f};
    } else if (render_perspective_ == player_id) {
      return glm::vec4{0.72f, 0.22f, 0.52f, 1.0f};
    } else {
      return glm::vec4{1.0f, 0.5f, 0.5f, 1.0f};
    }
  }
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit
