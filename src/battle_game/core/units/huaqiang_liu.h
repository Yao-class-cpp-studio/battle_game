#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class HuaqLiu : public Unit {
 public:
  HuaqLiu(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Move(float move_speed, float rotate_angular_speed);
  void KnifeRotate();
  void Split();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

 private:
  float knife_rotation_{0.0f};
  bool go_left_{true};
  bool has_knife_{false};
  bool has_hit_{false};
  float damage_scale_{1.0f};
  glm::vec2 knife_offset_{-0.4f, -0.4f};
  float knife_length_{2.0f};
};
}  // namespace battle_game::unit
