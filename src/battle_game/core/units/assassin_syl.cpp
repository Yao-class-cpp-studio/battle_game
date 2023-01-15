#include "assassin_syl.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
Assassin_syl::Assassin_syl(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Invincible";
  temp.description = "You are invincible for 5 seconds";
  temp.time_remain = 0;
  temp.time_total = 480;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(Assassin_syl::InvincibleClick);
  skills_.push_back(temp);
  temp.name = "Chibaku Tensei";
  temp.description =
      "If you are invincible, then you next attack will be a "
      "super attack whose damage_scale is 20";
  temp.time_remain = 0;
  temp.time_total = 1200;
  temp.type = Q;
  temp.function = SKILL_ADD_FUNCTION(Assassin_syl::ChibakuTenseiClick);
  skills_.push_back(temp);
  temp.name = "Sneak";
  temp.description =
      "You move speed will increase 2 percent for every 1 "
      "percent of health lost compared to maximum health,when you are "
      "invincible";
}

void Assassin_syl::Render() {
  Tank::Render();
}

void Assassin_syl::Update() {
  float move_speed = 6.0f;
  if (isInvincible) {
    move_speed *= (1.0f + (1.0f - health_) * 2.0f);
  }
  TankMove(move_speed, glm::radians(270.0f));
  TurretRotate();
  Invincible();
  ChibakuTensei();
  Fire();
}

void Assassin_syl::InvincibleClick() {
  isInvincible = 5 * kTickPerSecond;
  Invincible_count_down_ = 8 * kTickPerSecond;
}

void Assassin_syl::ChibakuTenseiClick() {
  ChibakuTensei_count_down_ = 20 * kTickPerSecond;
  isChibakuTensei = true;
}

void Assassin_syl::Invincible() {
  skills_[0].time_remain = Invincible_count_down_;
  if (Invincible_count_down_) {
    Invincible_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        InvincibleClick();
      }
    }
  }
  if (isInvincible) {
    isInvincible--;
  }
}

void Assassin_syl::ChibakuTensei() {
  skills_[1].time_remain = ChibakuTensei_count_down_;
  if (ChibakuTensei_count_down_) {
    ChibakuTensei_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        ChibakuTenseiClick();
      }
    }
  }
}

float Assassin_syl::GetDamageScale() {
  float damage_scale = 1;
  if (isChibakuTensei && isInvincible) {
    damage_scale = 20;
    isChibakuTensei = false;
  }
  return damage_scale;
}

void Assassin_syl::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool Assassin_syl::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *Assassin_syl::UnitName() const {
  return "Assassin";
}

const char *Assassin_syl::Author() const {
  return "syl";
}

}  // namespace battle_game::unit
