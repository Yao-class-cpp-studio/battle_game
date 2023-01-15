#include "square_tank.h"

#include <math.h>

#include <vector>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

using namespace std;

namespace battle_game::unit {
namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

SquareTank::SquareTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();

    vector<ObjectVertex> body_vertices;
    vector<uint32_t> body_indices;
    body_vertices.push_back(
        {{-0.7f, 0.7f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{-0.7f, -0.7f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{0.7f, 0.7f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{0.7f, -0.7f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_indices.push_back(0);
    body_indices.push_back(1);
    body_indices.push_back(2);
    body_indices.push_back(1);
    body_indices.push_back(2);
    body_indices.push_back(3);

    tank_body_model_index = mgr->RegisterModel(body_vertices, body_indices);

    vector<ObjectVertex> turret_vertices;
    vector<uint32_t> turret_indices;
    const int precision = 60;
    const float inv_precision = 1.0f / float(precision);
    const float pi_ = acos(-1);
    for (int i = 0; i < precision; ++i) {
      auto theta = (float(i) + 0.5f) * inv_precision;
      theta *= 2.0f * pi_;
      auto sin_theta = sin(theta);
      auto cos_theta = cos(theta);
      turret_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                 {0.0f, 0.0f},
                                 {0.5f, 0.5f, 0.5f, 1.0f}});
      turret_indices.push_back(i);
      turret_indices.push_back((i + 1) % precision);
      turret_indices.push_back(precision);
    }
    turret_vertices.push_back(
        {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
    turret_vertices.push_back(
        {{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
    turret_vertices.push_back(
        {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
    turret_vertices.push_back(
        {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
    turret_vertices.push_back(
        {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}});
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

void SquareTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void SquareTank::Update() {
  TankMove(5.0f);
  TurretRotate();
  Fire();
}

void SquareTank::TankMove(float move_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_A])
      offset.x -= 1.0f;
    if (input_data.key_down[GLFW_KEY_D])
      offset.x += 1.0f;
    if (input_data.key_down[GLFW_KEY_W])
      offset.y += 1.0f;
    if (input_data.key_down[GLFW_KEY_S])
      offset.y -= 1.0f;

    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position = position_ + offset;
    if (!game_core_->IsBlockedByObstacles(new_position))
      game_core_->PushEventMoveUnit(id_, new_position);
  }
}

void SquareTank::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4)
      turret_rotation_ = rotation_;
    turret_rotation_ = atan2(diff.y, diff.x) - glm::radians(90.0f);
  }
}

void SquareTank::Fire() {
  if (fire_count_down_)
    --fire_count_down_;
  else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_1])
        bullet_type = 1;
      else if (input_data.key_down[GLFW_KEY_2])
        bullet_type = 2;
      else if (input_data.key_down[GLFW_KEY_3])
        bullet_type = 3;
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),

            turret_rotation_, 1.0f / (float)bullet_type * GetDamageScale(),
            velocity * (float)bullet_type);
        fire_count_down_ = kTickPerSecond * 1.0f / (float)bullet_type;
      }
    }
  }
}

bool SquareTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.7f && position.x < 0.7f && position.y > -0.7f &&
         position.y < 0.7f;
}

const char *SquareTank::UnitName() const {
  return "Square Tank";
}

const char *SquareTank::Author() const {
  return "Ted";
}
}  // namespace battle_game::unit
