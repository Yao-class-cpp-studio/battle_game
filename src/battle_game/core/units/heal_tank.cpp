#include "heal_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit{
HealTank::HealTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
    Skill temp;
    temp.name = "Inhale";
    temp.description = "Refresh health by attacking enemies";
    temp.time_remain = 0;
    temp.time_total = 15 * kTickPerSecond;
    temp.type = E;
    temp.function = SKILL_ADD_FUNCTION(HealTank::InhaleClick);
    skills_.push_back(temp);
    temp.name = "Refresh";
    temp.description = "Refresh to full health";
    temp.time_remain = 0;
    temp.time_total = 45 * kTickPerSecond;
    temp.type = Q;
    temp.function = SKILL_ADD_FUNCTION(HealTank::RefreshClick);
    skills_.push_back(temp);
    health_ = 100.0f / 150;
    }

void HealTank::Render() {
  Tank::Render();
}

void HealTank::Update() {
  TankMove(4.0f, glm::radians(180.0f));
  TurretRotate();
  Heal();
  Expand();
  Inhale();
  Refresh();
  DealInhale();
  Fire();
}

float HealTank::GetDamageScale() const {
  return 0.6f;
}

float HealTank::GetHealthScale() const {
  return health_scale_;
}

float HealTank::BasicMaxHealth() const {
  return 150.0f;
}

void HealTank::Fire()
{
  if (isinhale_ && fire_count_down_ > 30)
    fire_count_down_ = 30;
  if (fire_count_down_)
    fire_count_down_--;
  else
  {
    auto player = game_core_->GetPlayer(player_id_);
    if (player)
    {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
      {
        if (isinhale_)
        {
          for (int i = -4; i <= 4; i++)
          {
            auto offset = glm::radians(7.5f * i);
            auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_
                                    + offset);
            GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_ + offset, 0.2f, velocity);
          }
        }
        else
        {
          for (int i = -2; i <= 2; i++)
          {
            auto offset = glm::radians(10.0f * i);
            auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_
                                    + offset);
            GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_ + offset, GetDamageScale(), velocity);
          }
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
  if (isinhale_)
    isinhale_--;
}

void HealTank::InhaleClick()
{
  isinhale_ = 5 * kTickPerSecond;
  inhale_count_down_ = 15 * kTickPerSecond;
}

void HealTank::RefreshClick()
{
  SetHealth(1.0f);
  refresh_count_down_ = 45 * kTickPerSecond;
}

void HealTank::Inhale()
{
  skills_[0].time_remain = inhale_count_down_;
  if (inhale_count_down_)
    inhale_count_down_--;
  else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player){
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E])
        InhaleClick();
    }
  }
}

void HealTank::Refresh()
{
  skills_[1].time_remain = refresh_count_down_;
  if (refresh_count_down_){
    refresh_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player){
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q])
        RefreshClick();
    }
  }
}

void HealTank::Heal()
{
  if (heal_count_down_)
    heal_count_down_--;
  else {
    SetHealth(GetHealth() + 0.1f / GetMaxHealth());
    heal_count_down_ = 5;
  }
}

void HealTank::Expand()
{
  if (expand_count_down_)
    expand_count_down_--;
  else {
    health_scale_ += 10.0f / BasicMaxHealth();
    expand_count_down_ = 15 * kTickPerSecond;
  }
}

void HealTank::DealInhale()
{
  if (isinhale_)
  {
    SetHealth(GetHealth() + attack_count_ * 2.0f / GetMaxHealth());
  }
  attack_count_ = 0;
}

bool HealTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *HealTank::UnitName() const {
  return "Heal Tank";
}

const char *HealTank::Author() const {
  return "lcl";
}
}  // namespace battle_game::unit
