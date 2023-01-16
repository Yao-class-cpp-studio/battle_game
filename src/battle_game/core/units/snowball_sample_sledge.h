#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Snowball_Sample_Sledge : public Tank {
 public:
  Snowball_Sample_Sledge(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
   
 protected:
  void Fire();
  void AntigravityClick();
  void Antigravity();
  void Antigravity_();
  void Antigravity_fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  uint32_t fire_count_down_{0};
  uint32_t antigravity_count_down_{360};
  uint32_t shoot_count_down_{0};
  uint32_t hidden_count_down_{181};
  float speed_{5.0f};

 private:
  uint32_t id_{0};
};
}  // namespace battle_game::unit