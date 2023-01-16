#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class phantom_of_cosin : public Tank {
 public:
  phantom_of_cosin(GameCore *game_core, uint32_t id, uint32_t player_id);
 
  void Update() override;


 protected:
  

  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
}  // namespace battle_game::unit