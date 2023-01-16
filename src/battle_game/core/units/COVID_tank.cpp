// cite: partially taken from "tiny_tank.cpp"
#include "COVID_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

CovidTank::CovidTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      tank_body_model_index = mgr->RegisterModel(
          {
              {{-1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{1.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2, 1, 2, 3});
    }

    {
      /* Tank Turret */
      tank_turret_model_index = mgr->RegisterModel(
          {{{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}},
           {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}},
           {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}},
           {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}}},
          {0, 1, 2, 1, 2, 3});
    }
  }
}

void CovidTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture("../../textures/colorful_COVID.png");
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void CovidTank::Update() {
  TankMove(10.0f, 0.0f);
  TurretRotate();
  Fire();
}

void CovidTank::TankMove(float move_speed, float rotate_angular_speed) {
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
    if (input_data.key_down[GLFW_KEY_A]) {
      offset.x -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      offset.x += 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position = position_ + offset;
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
  }
}

void CovidTank::TurretRotate() {
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

void CovidTank::Fire() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
      is_fire_ = true;
    }
    if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_RIGHT]) {
      is_fire_ = false;
    }
    if (is_fire_) {
      if (fire_count_down_) {
        fire_count_down_--;
      } else {
        auto position_turrent_mouse =
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_);
        for (int i = -half_num_fork_bullet_; i <= half_num_fork_bullet_; i++) {
          GenerateBullet<bullet::CovidBall>(
              position_turrent_mouse, turret_rotation_, GetDamageScale(),
              Rotate(glm::vec2{0.0f, bullet_velocicy_},
                     turret_rotation_ + i * diff_angle_fork_bullet_));
        }
        fire_count_down_ = kTickPerSecond / fire_speed_;
      }
    }
  }
}

bool CovidTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -1.0f && position.x < 1.0f && position.y > -1.0f &&
         position.y < 1.0f;
}

float CovidTank::GetBulletGravity() const {
  return bullet_gravity_;
}

const char *CovidTank::UnitName() const {
  return "COVID Tank";
}

const char *CovidTank::Author() const {
  return "hwj";
}
}  // namespace battle_game::unit
