#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {

class CoolTank : public Tank {
 public:
  CoolTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  virtual void Addhitnumber();
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void CoolFire();
  int hit_number_ = 0;
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

};
}  // namespace battle_game::unit