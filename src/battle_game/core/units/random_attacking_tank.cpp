#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include <iostream>

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

RandomAttackingTank::RandomAttackingTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      std::vector<ObjectVertex> body_vertices;
      std::vector<uint32_t> body_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        body_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                 {0.0f, 0.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f}});
        body_indices.push_back(i);
        body_indices.push_back((i + 1) % precision);
        body_indices.push_back(precision);
      }
      tank_body_model_index = mgr->RegisterModel(body_vertices, body_indices);
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
        turret_vertices.push_back({{sin_theta * 0.25f, cos_theta * 0.25f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.05f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.05f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.05f, 0.6f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.05f, 0.6f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
    turret_rotation_ = glm::radians(game_core_->RandomFloat() * 360.0f);
    rotation_ = glm::radians(game_core_->RandomFloat() * 360.0f);
  }
}

void RandomAttackingTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void RandomAttackingTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void RandomAttackingTank::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    glm::vec2 offset{0.0f};
    offset.y += 0.5f + game_core_->RandomFloat() * 0.5;
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    float rotation_offset = 0.0f;
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    } 
    else {
      auto blocked_obstacle = game_core_->GetBlockedObstacle(new_position);
      if (blocked_obstacle != nullptr){
        auto normal = blocked_obstacle->GetSurfaceNormal(position_, new_position);
        rotation_offset = - 2 * rotation_ 
          + 2 * std::atan2(normal.second.y, normal.second.x);
      }
    }
    if (move_rotation_count_down_) {
      move_rotation_count_down_--;
    } 
    else {
      rotation_offset += 
        glm::radians(game_core_->RandomFloat() * 360.0f);
      rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
      move_rotation_count_down_ = kTickPerSecond/3;
    }
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void RandomAttackingTank::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    bool is_anti_clockwise = change_of_turret_rotation_ > 0;
    if (change_of_turret_rotation_ > glm::radians(5.0f)) {
      turret_rotation_count_down_ = 15;
    } else if (change_of_turret_rotation_ < glm::radians(-5.0f) ) {
      turret_rotation_count_down_ = 15;
    }
    if (turret_rotation_count_down_) {
      turret_rotation_count_down_--;
      if (is_anti_clockwise){
        change_of_turret_rotation_ -= glm::radians(0.4f);
      }
      else {
        change_of_turret_rotation_ += glm::radians(0.4f);
      }
    } 
    else {
      change_of_turret_rotation_ += 
        glm::radians((game_core_->RandomFloat() - 0.5f) * 1.2f);
    }
    turret_rotation_ += change_of_turret_rotation_;
  }
}

void RandomAttackingTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = 0.8*kTickPerSecond;  // Fire interval 0.8 second.
      }
    }
  }
}

bool RandomAttackingTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return pow(position.x, 2) + pow(position.y, 2) <= 0.25f;
}

float RandomAttackingTank::GetSpeedScale() const {
  return 0.8f;
}

float RandomAttackingTank::GetDamageScale() const {
  return 0.3f;
}

float RandomAttackingTank::BasicMaxHealth() const {
  return 30.0f;
}

const char *RandomAttackingTank::UnitName() const {
  return "Random Attacking Tank";
}

const char *RandomAttackingTank::Author() const {
  return "Chen Rui Qin Fang";
}
}  // namespace battle_game::unit
