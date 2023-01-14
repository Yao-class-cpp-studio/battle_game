#include "magic_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

MagicTank::MagicTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  {
    /* Tank skill protect*/
    Skill temp;
    temp.name = "Protect";
    temp.description = "Prevent any possible damage";
    temp.time_remain = 0;
    temp.time_total = 60;
    temp.type = E;
    temp.function = SKILL_ADD_FUNCTION(MagicTank::Protect);
    skills_.push_back(temp);
    Skill temp2;
    temp2.name = "Death";
    temp2.description = "Shoot deathball to enemies";
    temp2.time_remain = 0;
    temp2.time_total = 240;
    temp2.type = Q;
    temp2.function = SKILL_ADD_FUNCTION(MagicTank::Protect);
    skills_.push_back(temp2);
  }
}

void MagicTank::Render() {
  Tank::Render();
}

void MagicTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
  Protect();
  Death();
  magic();
}

void MagicTank::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void MagicTank::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void MagicTank::Fire() {
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

void MagicTank::ProtectClick() {
  protecteed = true;
  protectskill_countdown_ = 20 * kTickPerSecond;
}

void MagicTank::DeathClick() {
  deathcool_countdown_ = 240;
  health_ /= 2;  // Death should have some side effect
  auto velocity = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_);
  GenerateBullet<bullet::Deathball>(
      position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
      GetDamageScale(), velocity);
}

void MagicTank::magic() {
  if (protecteed) {
    if (protectskill_countdown_) {
      protectskill_countdown_--;
      if (protectskill_countdown_ == 0) {
        protecteed = false;
        protectcool_countdown = 60 * kTickPerSecond;
      }
    }
  }
}

void MagicTank::Protect() {
  skills_[0].time_remain = protectcool_countdown;
  if (protectcool_countdown) {
    protectcool_countdown--;
  } else if (protecteed == false) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        ProtectClick();
      }
    }
  }
}

void MagicTank::Death() {
  skills_[1].time_remain = deathcool_countdown_;
  if (deathcool_countdown_) {
    deathcool_countdown_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        DeathClick();
      }
    }
  }
}

bool MagicTank::IsHit(glm::vec2 position) const {
  if (protecteed == true)
    return false;  // Under Protected
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *MagicTank::UnitName() const {
  return "Magic Tank";
}

const char *MagicTank::Author() const {
  return "PorridgeSailing";
}
}  // namespace battle_game::unit
