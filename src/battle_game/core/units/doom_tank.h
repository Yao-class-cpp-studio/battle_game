#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class DoomTank : public Tank {
 public:
  DoomTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  //void Crater();
  void RandomDo();
  void Doomsday();
  void DestroyAll();
  void DoomTankUpdate();
  uint32_t unit_id_{};
  uint32_t id_{0};
  bool IsHit(glm::vec2 position) const;
  uint32_t doomsday_count_down_{60};
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit