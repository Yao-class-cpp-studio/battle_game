#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/input_data.h"

namespace battle_game::unit {
class SpellCaster : public Unit {
 public:
  SpellCaster(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;

 protected:
  [[nodiscard]] const char *SpellCaster::Author() const override { return "BAN_43_32532"; }
  float getCursorDirection();
  template <class BulletType>
  void Fire(float direction, float speed);
  void Move();
  virtual void Spell();
  enum {
    Arrow,
    WASD,
    Mouse } move_mode_{Arrow};
  bool auto_move_{false};
  bool auto_spell_{true};
  InputData input_;
  InputData last_input_;
  InputData input_to_act_;
  float hitbox_radius_{0.5f};
  float speed_{6.0f};
};

class Udongein : public SpellCaster {
 public:
  Udongein(GameCore *game_core, uint32_t id, uint32_t player_id);
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  [[nodiscard]] const char *UnitName() const override { return "Udongein"; }
  void Spell() override;
  void DirectionalFire();
  void BoundaryBetweenWavesAndParticles();
  struct {
    uint32_t clock_{0};
  } directional_fire;
  struct {
    uint32_t clock_{0};
    float angle_{0.0f};
    float angle_increment_{glm::radians(1.0f)};
  } boundary_between_waves_and_particles;
};

}  // namespace battle_game::unit
