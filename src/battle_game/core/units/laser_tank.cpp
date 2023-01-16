#include "laser_tank.h"

#include <iostream>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/bullets/laser.h"
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
        if (isDestroy_ != 0) {
          DestroyClick();
        } else {
          auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
          if (!iscombo_) {
            iscombo_ = true;
            for (int i = 1; i <= 6; i++) {
              GenerateBullet<bullet::Laser>(
                  glm::vec2{0.0f, 0.0f}, turret_rotation_,
                  GetDamageScale() * 0.25f, velocity, i, 1);
            }
          }
        }
      } else {
        fire_count_down_ = kTickPerSecond / 3;
        if ((iscombo_ == true) && (isDestroy_ != 0)) {
          isDestroy_ = 0;
        }
        iscombo_ = false;
      }
    }
  }
}

void Laser_Tank::DestroyClick() {
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
  if (!iscombo_) {
    iscombo_ = true;
    GenerateBullet<bullet::Laser>(glm::vec2{0.0f, 0.0f}, turret_rotation_,
                                  GetDamageScale() * 4.0f, velocity, 1, 2);
  }
}

void Laser_Tank::Destroy() {
  skills_[0].time_remain = destroy_count_down_;
  if ((destroy_count_down_ != 0) && (isDestroy_ == 0)) {
    destroy_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E] &&
          !input_data.key_down[GLFW_MOUSE_BUTTON_LEFT]) {
        isDestroy_ = 1;
        destroy_count_down_ = 10 * kTickPerSecond;
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
  temp.time_total = 600;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(Laser_Tank::Destroy);
  skills_.push_back(temp);
}

void Laser_Tank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
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
