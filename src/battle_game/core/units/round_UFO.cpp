#include "round_UFO.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

RoundUFO::RoundUFO(GameCore *game_core, uint32_t id, uint32_t player_id)
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
  // Skill
  Skill Teleport;
  Teleport.name = "Teleport";
  Teleport.description = "Teleport to anywhere the mouse click.";
  Teleport.time_remain = 0;
  Teleport.time_total = kTickPerSecond * 10;
  Teleport.type = SkillType::E;
  Teleport.function = SKILL_ADD_FUNCTION(RoundUFO::Teleport_);
  skills_.push_back(Teleport);
}

void RoundUFO::Teleport() {
  skills_[0].time_remain = teleport_count_down_;
  if (teleport_count_down_) {
    teleport_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        Teleport_();
        teleport_count_down_ = 600;
      }
    }
  }
}

void RoundUFO::Teleport_() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto new_position = input_data.mouse_cursor_position;
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
  }
}

void RoundUFO::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void RoundUFO::Update() {
  UFOMove(10.0f);
  Teleport();
  Fire();
}

void RoundUFO::UFOMove(float move_speed) {
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

    auto diff = input_data.mouse_cursor_position - position_;
    float new_rotation;
    if (glm::length(diff) < 1e-4) {
      new_rotation = rotation_;
    } else {
      new_rotation = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
    game_core_->PushEventRotateUnit(id_, new_rotation);
  }
}

void RoundUFO::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        auto angle = 360.0f / 20;
        for (int i = -10; i < 10; ++i) {
          auto angle_offset = glm::radians(angle * i);
          velocity =
              Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + angle_offset);
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + angle_offset),
              turret_rotation_ + angle_offset, GetDamageScale(), velocity);
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool RoundUFO::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return glm::length(position) < 1.0f;
}

const char *RoundUFO::UnitName() const {
  return "Round UFO";
}

const char *RoundUFO::Author() const {
  return "Fireond";
}
}  // namespace battle_game::unit
