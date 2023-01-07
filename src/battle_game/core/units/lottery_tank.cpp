#include "lottery_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
LotteryTank::LotteryTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void LotteryTank::Render() {
  Tank::Render();
}
void LotteryTank::Update() {
  Tank::TankMove(3.0f, glm::radians(180.0f));
  Tank::TurretRotate();
  Fire();
}

void LotteryTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        switch (GetBulletType()) {
          case BigScatter:
            ScatterFire(ScatterBulletsCount, true);
            break;
          case Scatter:
            ScatterFire(ScatterBulletsCount, false);
            break;
          case Big:
            ScatterFire(NormalBulletsCount, true);
            break;
          case Normal:
          dafault:
            ScatterFire(NormalBulletsCount, false);
            break;
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool LotteryTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

bool LotteryTank::IsBig() {
  return CardLottery(GetNormalTimes, InitProb_Big, BigBulletFloor,
                     BigBulletCeil);
}

bool LotteryTank::IsScatter() {
  return CardLottery(GetOnceTimes, InitProb_Scatter, ScatterBulletFloor,
                     ScatterBulletCeil);
}

LotteryTank::BulletType LotteryTank::GetBulletType() {
  uint32_t Type = IsBig() | (IsScatter() << 1);
  if (Type & 1)
    GetNormalTimes = 0;
  else
    ++GetNormalTimes;

  if (Type & 2)
    GetOnceTimes = 0;
  else
    ++GetOnceTimes;

  switch (Type) {
    case 1:
      return Big;
    case 2:
      return Scatter;
    case 3:
      return BigScatter;
    case 0:
    default:
      return Normal;
  }
}

void LotteryTank::ScatterFire(uint32_t BulletsCount, bool IsBig) {
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
  float Damage =
      GetDamageScale() * (IsBig ? BigDamageRange : NormalDamageRange);
  int lim = BulletsCount - 1;
  for (int i = -lim; i <= lim; i += 2) {
    auto offset = glm::radians(3.0f * i);
    velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
    if (IsBig)
      GenerateBullet<bullet::Rocket>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
          turret_rotation_ + offset, Damage, velocity);
    else
      GenerateBullet<bullet::CannonBall>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
          turret_rotation_ + offset, Damage, velocity);
  }
}

uint32_t LotteryTank::MinusClamp(uint32_t x, uint32_t y, uint32_t floor) {
  return x < y + floor ? floor : x - y;
}

bool LotteryTank::CardLottery(uint32_t times,
                              float init_prob,
                              uint32_t floor,
                              uint32_t ceil) {
  float rand_num_ = game_core_->RandomFloat();
  return rand_num_ <=
         init_prob + (1.0f - init_prob) * (MinusClamp(times, floor, 0) * 1.0f /
                                           MinusClamp(ceil, floor, 1));
}

const char *LotteryTank::UnitName() const {
  return "Lottery Tank";
}

const char *LotteryTank::Author() const {
  return "JasonL";
}
}  // namespace battle_game::unit
