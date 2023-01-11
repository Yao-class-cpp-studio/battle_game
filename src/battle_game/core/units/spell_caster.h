#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/input_data.h"

namespace battle_game::unit {
class SpellCaster : public Unit {
 public:
  SpellCaster(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  [[nodiscard]] const char *SpellCaster::Author() const override { return "BAN_43_32532"; }
  float getCursorDirection(glm::vec2 mouse_position);
  template <class BulletType>
  void Fire(float direction, float speed) {
    auto velocity = Rotate(glm::vec2{0.0f, speed}, direction);
    GenerateBullet<BulletType>(position_ + Rotate({0.0f, 0.5f}, direction),
                               direction, GetDamageScale(), velocity);
  }
  float CheckSlowMode(float speed);
  void Move();
  virtual void Spell();
  enum {
    Arrow,
    WASD,
    Mouse } move_mode_{Arrow};
  bool auto_move_{false};
  bool auto_spell_{true};
  InputData input_{};
  InputData last_input_{};
  InputData input_to_act_{};
  float hitbox_radius_{0.5f};
  float speed_{6.0f};
};
}  // namespace battle_game::unit