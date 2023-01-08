#pragma once
#include "tiny_tank.h"

namespace battle_game::unit {
class LotteryTank : public Tank {
  // Basic Settings
 public:
  LotteryTank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
#define next_bullet_ skills_[0];
  uint32_t next_bullet_type_{0};

  // Skill Settings
  Skill passive_skill_info_;
  void ResetSkillInfo();

  // Bullet Settings
  std::vector<Skill> bullet_info_;
  void ResetBulletInfo();
  const float normal_damage_range_{1.0f};
  const float big_damage_range_{10.0f};
  const uint32_t normal_bullets_count_{1};
  const uint32_t scatter_bullets_count_{5};
  bool IsBig();
  bool IsScatter();
  uint32_t GetNextBulletType();
  void SwitchBullet(uint32_t type);
  void ScatterFire(uint32_t BulletsCount, bool IsBig);

  // Lottery Probability Settings
  uint32_t get_normal_times_{1};
  uint32_t get_once_times_{1};
  const uint32_t big_bullet_floor_{10};
  const uint32_t big_bullet_ceil_{20};
  const uint32_t scatter_bullet_floor_{20};
  const uint32_t scatter_bullet_ceil_{50};
  const float init_prob_big_{0.2f};
  const float init_prob_scatter_{0.02f};
  uint32_t MinusClamp(uint32_t x, uint32_t y, uint32_t floor);
  bool CardLottery(uint32_t times,
                   float init_prob,
                   uint32_t floor,
                   uint32_t ceil);
};
}  // namespace battle_game::unit
