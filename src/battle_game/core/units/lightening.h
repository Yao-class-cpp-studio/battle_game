#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Lightening : public Tank {
 public:
  Lightening(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  void RocketClick();
  void Rocket();
  bool Flash();
  void FlashClick();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t fire_count_down_{0};

  uint32_t rocket_count_down_{0};
  uint32_t flash_duration_{0};

  uint32_t flash_count_down_{0};
};
}  // namespace battle_game::unit
