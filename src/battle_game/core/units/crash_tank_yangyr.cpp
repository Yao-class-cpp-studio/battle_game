#include "crash_tank_yangyr.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

CrashTank::CrashTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      std::vector<ObjectVertex> turret_vertices;
      std::vector<ObjectVertex> body_vertices;
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
        body_vertices.push_back({{sin_theta * 1.0f, cos_theta * 1.0f},
                                 {0.0f, 0.0f},
                                 {1.0f, 1.0f, 1.0f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
      tank_body_model_index = mgr->RegisterModel(body_vertices, turret_indices);
    }
  }
}

void CrashTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void CrashTank::Update() {
  Move(10.0f, glm ::radians(180.0f));
  TurretRotate();
  Crash();
}

void CrashTank::Move(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f,0.0f};
    if (input_data.key_down[GLFW_KEY_A]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      offset.y -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.x += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.x -= 1.0f;
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

void CrashTank::Crash() {
  if (is_crashed_ == false && is_crashing_ == true) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
      GenerateBullet<bullet::CannonBall>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
          GetDamageScale(), velocity);
    }
    is_crashed_ = true;
  }
}

glm::highp_vec2 CrashTank::FindEnemy() {
  auto &units = game_core_->GetUnits();
  auto it = units.begin();
  auto itEnd = units.end();
  auto diff = position_ - position_;
  while (glm::length(diff) < 1e-4 && it != itEnd) {
    diff = (it)->second->GetPosition() - position_;
    it++;
  }
  auto next_diff = diff;
  while (it != itEnd) {
    next_diff = (it)->second->GetPosition() - position_;
    if (glm::length(next_diff) >= 1e-4)
      diff = glm::length(next_diff) < glm::length(diff) ? next_diff : diff;
    it++;
  }
  if (glm::length(diff) < 2.0f) {
    is_crashing_ = true;
    return diff;
  } else {
    is_crashed_ = false;
    is_crashing_ = false;
    return position_ - position_;
  }
}

void CrashTank::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto diff = FindEnemy();
    if (glm::length(diff) < 1e-4) {
      // turret_rotation_ = rotation_;
      fire_count_down_ = 1;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

bool CrashTank::IsHit(glm::vec2 position) const {
  if (is_crashed_ == false && is_crashing_ == true)
    return true;
  position = WorldToLocal(position);
  return glm::length(position) < 1.0f;
}

const char *CrashTank::UnitName() const {
  return "Crash Tank";
}

const char *CrashTank::Author() const {
  return "YYR";
}
}  // namespace battle_game::unit
