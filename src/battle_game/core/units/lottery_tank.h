#pragma once
#include "battle_game/core/unit.h"
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

  // Bullet Settings
  enum BulletType {
    Normal = 0,  // 1.0x
    Big,         // 10.0x
    Scatter,     // 1.0x, 5 bullets
    BigScatter   // 10.0x, 5 bullets
  };
  const float NormalDamageRange{1.0f};
  const float BigDamageRange{10.0f};
  const uint32_t NormalBulletsCount{1};
  const uint32_t ScatterBulletsCount{5};
  bool IsBig();
  bool IsScatter();
  BulletType GetBulletType();
  void ScatterFire(uint32_t BulletsCount, bool IsBig);

  // Lottery Probability Settings
  uint32_t GetNormalTimes{0};
  uint32_t GetOnceTimes{0};
  const uint32_t BigBulletFloor{10};
  const uint32_t BigBulletCeil{20};
  const uint32_t ScatterBulletFloor{20};
  const uint32_t ScatterBulletCeil{50};
  const float InitProb_Big{0.2f};
  const float InitProb_Scatter{0.02f};
  uint32_t MinusClamp(uint32_t x, uint32_t y, uint32_t floor);
  bool CardLottery(uint32_t times,
                   float init_prob,
                   uint32_t floor,
                   uint32_t ceil);
};
}  // namespace battle_game::unit
