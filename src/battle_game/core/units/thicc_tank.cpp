#include "thicc_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t thicc_tank_body_model_index = 0xffffffffu;
uint32_t thicc_tank_turret_model_index = 0xffffffffu;
}  // namespace

ThiccTank::ThiccTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~thicc_tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      thicc_tank_body_model_index = mgr->RegisterModel(
          {
              {{-1.6f, 1.6f}, {0.0f, 0.0f}, {1.0f, 0.5f, 0.5f, 1.0f}},
              {{-1.6f, -2.0f}, {0.0f, 0.0f}, {1.0f, 0.5f, 0.5f, 1.0f}},
              {{1.6f, 1.6f}, {0.0f, 0.0f}, {1.0f, 0.5f, 0.5f, 1.0f}},
              {{1.6f, -2.0f}, {0.0f, 0.0f}, {1.0f, 0.5f, 0.5f, 1.0f}},
              {{1.2f, 2.0f}, {0.0f, 0.0f}, {1.0f, 0.5f, 0.5f, 1.0f}},
              {{-1.2f, 2.0f}, {0.0f, 0.0f}, {1.0f, 0.5f, 0.5f, 1.0f}},
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
        turret_vertices.push_back({{sin_theta * 1.0f, cos_theta * 1.0f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.5f, 0.5f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{-0.2f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{0.2f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{-0.2f, 2.4f}, {0.0f, 0.0f}, {0.7f, 0.5f, 0.5f, 1.0f}});
      turret_vertices.push_back(
          {{0.2f, 2.4f}, {0.0f, 0.0f}, {0.7f, 0.5f, 0.5f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      thicc_tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
  Skill temp;
  temp.name = "Death Trap";
  temp.description = "Explosion upon death";
  temp.time_remain = 0;
  temp.time_total = 0;
  temp.type = P;
  skills_.push_back(temp);
}

void ThiccTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(thicc_tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(thicc_tank_turret_model_index);
}

void ThiccTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

float ThiccTank::GetSpeedScale() const {
  return 0.25f;
}

float ThiccTank::GetHealthScale() const {
  return 4.0f;
}

void ThiccTank::TankMove(float move_speed, float rotate_angular_speed) {
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

void ThiccTank::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 2e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void ThiccTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 40.0f}, turret_rotation_);  // Faster bullet than normal
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = 2 * kTickPerSecond;  // Fire interval 2 second.
      }
    }
  }
}

void ThiccTank::DeathCall(uint32_t src_unit_id) {
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == id_) {
      continue;
    }
    auto enemy_pos = units.at(unit.first)->GetPosition();
    enemy_pos = WorldToLocal(enemy_pos);
    if ((abs(enemy_pos.x) + abs(enemy_pos.y)) < 10.0) {
      game_core_->PushEventDealDamage(
          unit.first, id_, 100.0f - 10 * abs(enemy_pos.x) - 10 * abs(enemy_pos.y));
      game_core_->PushEventGenerateParticle<particle::Explosion>(
          position_, rotation_, kTickPerSecond);
    }
  }
}

bool ThiccTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -1.6f && position.x < 1.6f && position.y > -2.0f &&
         position.y < 2.0f && position.x + position.y < 3.2f &&
         position.y - position.x < 3.2f;
}

const char *ThiccTank::UnitName() const {
  return "Tank but THICC";
}

const char *ThiccTank::Author() const {
  return "Lomiaya";
}
}  // namespace battle_game::unit
