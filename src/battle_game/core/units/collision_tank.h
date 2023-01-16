#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class CollisionTank : public Tank {
 public:
  CollisionTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  void InitAlreadyHit();
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void AlreadyHit();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  std::map<uint32_t, bool> alreadyhit_;
};
}  // namespace battle_game::unit
