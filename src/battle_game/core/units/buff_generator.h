#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class BuffGenerator : public Unit {
 public:
  BuffGenerator(GameCore *game_core, uint32_t id, uint32_t player_id = 0);
  void Update() override;
  void Render() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  void RenderLifeBar() override;

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

 private:
  void BuffGenerate();
  uint32_t buff_count_down_{1200};
  uint32_t buff_id_{0};
  Unit *buff_{nullptr};
};
}  // namespace battle_game::unit
