#include "battle_game/core/units/pacman.h"

#include <cstdio>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

PacMan::PacMan(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      tank_body_model_index = mgr->RegisterModel(
          {
              {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{1.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 3, 1, 2, 3});
    }
  }
}

void PacMan::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetRotation(turret_rotation_ + glm::radians(90.0f));
  if (pacman_ == 1 || pacman_ == 2 || pacman_ == 3)
    battle_game::SetTexture("../../textures/pacman-1.png");
  if (pacman_ == 4 || pacman_ == 5 || pacman_ == 6)
    battle_game::SetTexture("../../textures/pacman-2.png");
  if (pacman_ == 7 || pacman_ == 8 || pacman_ == 9)
    battle_game::SetTexture("../../textures/pacman-3.png");
  if (pacman_ == 10 || pacman_ == 11 || pacman_ == 12)
    battle_game::SetTexture("../../textures/pacman-4.png");
  if (pacman_ == 12)
    pacman_ = 1;
  else if (pacman_ != 1)
    pacman_ = pacman_ + 1;
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
}

void PacMan::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
  Check();
}

void PacMan::Check() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player && pacman_ == 6) {
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    GenerateBullet<bullet::CannonBall>(
        position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
        GetDamageScale(), velocity);
    fire_count_down_ = kTickPerSecond / 4;
  }
}

void PacMan::TurretRotate() {
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

void PacMan::TankMove(float move_speed, float rotate_angular_speed) {
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
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
  }
}

void PacMan::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
        pacman_ = 2;
    }
  }
}

bool PacMan::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *PacMan::UnitName() const {
  return "Pac Man";
}

const char *PacMan::Author() const {
  return "LJFYC007";
}
}  // namespace battle_game::unit
