#include "Laser_Tank.h"

#include <iostream>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xfffffffu;
}  // namespace

void Laser_Tank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        if (isDestroy != 0) {
          DestroyClick();
        } else {
          auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
          if (!iscombo) {
            iscombo = true;
            for (int i = 1; i <= 6; i++) {
              GenerateBullet<bullet::Laser>(
                  glm::vec2{0.0f, 0.0f}, turret_rotation_,
                  GetDamageScale() * 0.25f, velocity, i, 1);
            }
          }
        }
      } else {
        fire_count_down_ = kTickPerSecond / 3;
        if ((iscombo == true) && (isDestroy != 0)) {
          isDestroy = 0;
        }
        iscombo = false;
      }
    }
  }
}

void Laser_Tank::DestroyClick() {
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
  if (!iscombo) {
    iscombo = true;
    GenerateBullet<bullet::Laser>(glm::vec2{0.0f, 0.0f}, turret_rotation_,
                                  GetDamageScale() * 3.6f, velocity, 1, 2);
  }
}

void Laser_Tank::Destroy() {
  skills_[0].time_remain = destroy_count_down_;
  if ((destroy_count_down_ != 0) && (isDestroy == 0)) {
    destroy_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E] &&
          !input_data.key_down[GLFW_MOUSE_BUTTON_LEFT]) {
        isDestroy = 1;
        destroy_count_down_ = 20 * kTickPerSecond;
      }
    }
  }
}

Laser_Tank::Laser_Tank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Destroy";
  temp.description = "Huge Damage";
  temp.time_remain = 0;
  temp.time_total = 1200;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(Laser_Tank::Destroy);
  skills_.push_back(temp);
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      tank_body_model_index = mgr->RegisterModel(
          {{{-0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3});
    }

    {
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

void Laser_Tank::Render() {
  Tank::Render();
}

void Laser_Tank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Destroy();
  Fire();
}

bool Laser_Tank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *Laser_Tank::UnitName() const {
  return "Laser Tank";
}

const char *Laser_Tank::Author() const {
  return "Qht Zkx Ljy";
}
}  // namespace battle_game::unit
