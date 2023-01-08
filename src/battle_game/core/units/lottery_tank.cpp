#include "lottery_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
LotteryTank::LotteryTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  ResetSkillInfo();
  ResetBulletInfo();
  skills_.push_back(bullet_info_[0]);
  skills_.push_back(passive_skill_info_);
}

void LotteryTank::ResetSkillInfo() {
  passive_skill_info_.name = u8"Lottery Time！";
  passive_skill_info_.description =
      u8"发射子弹时进行检定，若检定成功，子弹伤害倍率变大/转换为散射弹。";
  passive_skill_info_.time_remain = 0;
  passive_skill_info_.time_total = 0;
  passive_skill_info_.type = P;
}

void LotteryTank::ResetBulletInfo() {
  Skill temp;
  temp.time_remain = 0;
  temp.time_total = kTickPerSecond;  // Fire interval 1 second.
  temp.bullet_total_number = 4;
  temp.type = B;

  temp.name = u8"普通弹药";
  temp.description = u8"对敌人造成10点伤害。";
  temp.bullet_type = 1;
  bullet_info_.push_back(temp);

  temp.name = u8"暴击星";
  temp.description = u8"对敌人造成100点伤害。";
  temp.bullet_type = 2;
  bullet_info_.push_back(temp);

  temp.name = u8"爆裂弹";
  temp.description =
      u8"向前方扇形区域发射 5 发普通弹药，每发对敌人造成 10 点伤害。";
  temp.bullet_type = 3;
  bullet_info_.push_back(temp);

  temp.name = "SSR!";
  temp.description =
      u8"向前方扇形区域发射 5 发暴击星，每发对敌人造成 100 点伤害。";
  temp.bullet_type = 4;
  bullet_info_.push_back(temp);
}

void LotteryTank::SwitchBullet(uint32_t type) {
  skills_[0] = bullet_info_[type];
  skills_[0].time_remain = skills_[0].time_total;
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
  if (skills_[0].time_remain) {
    skills_[0].time_remain--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        switch (next_bullet_type_) {
          case 3:
            ScatterFire(scatter_bullets_count_, true);
            break;
          case 2:
            ScatterFire(scatter_bullets_count_, false);
            break;
          case 1:
            ScatterFire(normal_bullets_count_, true);
            break;
          case 0:
          dafault:
            ScatterFire(normal_bullets_count_, false);
            break;
        }
        next_bullet_type_ = GetNextBulletType();
        SwitchBullet(next_bullet_type_);
      }
    }
  }
}

bool LotteryTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

bool LotteryTank::IsBig() {
  return CardLottery(get_normal_times_, init_prob_big_, big_bullet_floor_,
                     big_bullet_ceil_);
}

bool LotteryTank::IsScatter() {
  return CardLottery(get_once_times_, init_prob_scatter_, scatter_bullet_floor_,
                     scatter_bullet_ceil_);
}

uint32_t LotteryTank::GetNextBulletType() {
  uint32_t type = IsBig() | (IsScatter() << 1);
  if (type & 1)
    get_normal_times_ = 0;
  else
    ++get_normal_times_;

  if (type & 2)
    get_once_times_ = 0;
  else
    ++get_once_times_;
  return type;
}

void LotteryTank::ScatterFire(uint32_t bullets_count, bool is_big) {
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
  float damage =
      GetDamageScale() * (is_big ? big_damage_range_ : normal_damage_range_);
  int lim = bullets_count - 1;
  for (int i = -lim; i <= lim; i += 2) {
    auto offset = glm::radians(3.0f * i);
    velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
    GenerateBullet<bullet::CannonBall>(
        position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
        turret_rotation_ + offset, damage, velocity);
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
