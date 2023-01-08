#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class HiddenTreasure : public Unit {
 public:
  HiddenTreasure(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  ~HiddenTreasure();
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  int obstacle_id;
  int type_id;
};
}  // namespace battle_game::unit
