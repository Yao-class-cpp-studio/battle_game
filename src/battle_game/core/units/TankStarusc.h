#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class TankStarusc : public Tank {
 public:
  TankStarusc(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  // [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] bool CheckDamage(glm::vec2 position,
                                 glm::vec2 velocity) const override;

 protected:
  // void TankMove(float move_speed, float rotate_angular_speed);
  // void TurretRotate();
  void Fire();
  void Sheild();
  void SheildClick();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  // float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_{0};
  uint32_t fire_count_{0};
  float fire_theta_{0};
  uint32_t sheild_count_down_{0}, have_sheild_{0};
  // uint32_t mine_count_down_{0};

 private:
  const float eps_starusc_ = 1e-6;
  float Vec2Cross(glm::vec2 x, glm::vec2 y) const;
};
}  // namespace battle_game::unit
