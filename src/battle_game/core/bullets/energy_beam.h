#pragma once
#include <utility>

#include "battle_game/core/bullet.h"
#include "battle_game/core/unit.h"

namespace battle_game::bullet {
class EnergyBeam : public Bullet {
  enum HitResultType {
    Miss,
    Obstacle,
    Unit,
  };
  struct HitResult {
    HitResultType hit_type;
    battle_game::Unit *unit;
    glm::vec2 position;
  };

 public:
  EnergyBeam(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale);
  ~EnergyBeam() override;
  void Render() override;
  void Update() override;

 protected:
  HitResult Target() const;
};
}  // namespace battle_game::bullet
