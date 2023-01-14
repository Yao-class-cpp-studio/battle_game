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
  bool ShouldLeech = false;
  void Leech();
  void Leech_();
  void ToLeech();
  uint32_t Damage{0};
  uint32_t unit_id_{};
  uint32_t id_{0};
  bool IsHit(glm::vec2 position) const;
  bool flag = false;
  bool IsLeeched = false;
  uint32_t doomsday_count_down_{0};
  uint32_t leech_count_down_{0};
  uint32_t leech_time_{240};
  uint32_t leech_volume_{0};
  double begin=1;
  double end=0;
  double tmp_begin;
  double tmp_end;
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit