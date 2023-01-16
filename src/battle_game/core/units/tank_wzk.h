#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class Tankwzk : public Tank {
 public:
  Tankwzk(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
};
<<<<<<< HEAD
}  // namespace battle_game::unit
=======
}  // namespace battle_game::unit
>>>>>>> efc4d4f1f3df6ae4324ba100220aeb44c9fb690b
