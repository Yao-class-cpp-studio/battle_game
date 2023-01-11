#pragma once
#include "battle_game/core/units/spell_caster.h"
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Udongein : public SpellCaster {
 public:
  Udongein(GameCore *game_core, uint32_t id, uint32_t player_id);

 protected:
  [[nodiscard]] const char *UnitName() const override {
    return "Udongein";
  }
  void Spell() override;
  void DirectionalFire();
  void BoundaryBetweenWavesAndParticles();
  struct {
    uint32_t clock{0};
  } directional_fire;
  struct {
    uint32_t line{};
    uint32_t clock{0};
    float angle{0.0f};
    float angle_increment{glm::radians(1.0f)};
  } boundary_between_waves_and_particles;
};
}  // namespace battle_game::unit