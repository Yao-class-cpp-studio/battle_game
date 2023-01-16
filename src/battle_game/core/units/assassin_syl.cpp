#include "assassin_syl.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

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
  temp.type = P;
  skills_.push_back(temp);

  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      tank_body_model_index = mgr->RegisterModel(
          {
              {{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              // distinguish front and back
              {{0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2, 1, 2, 3, 0, 2, 5, 2, 4, 5});
    }

    {
      /* Tank Turret */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
}

void Assassin_syl::Render() {
  if (isInvincible) {
    battle_game::SetTransformation(position_, rotation_);
    battle_game::SetTexture(0);
    battle_game::SetColor(glm::vec4{1.0f} -
                          game_core_->GetPlayerColor(player_id_));
    battle_game::DrawModel(tank_body_model_index);
    battle_game::SetRotation(turret_rotation_);
    battle_game::DrawModel(tank_turret_model_index);
  } else {
    Tank::Render();
  };
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
        if (isInvincible && isChibakuTensei) {
          GenerateBullet<bullet::HandSword>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
        } else {
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool Assassin_syl::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  if (isInvincible) {
    return false;
  }
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
