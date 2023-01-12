#include "neutral_turret.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t neutral_turret_model_index = 0xffffffffu;
}  // namespace

NeutralTurret::NeutralTurret(GameCore *game_core,
                             uint32_t id,
                             uint32_t player_id)
    : Unit(game_core, id, player_id) {
  auto mgr = AssetsManager::GetInstance();
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
    neutral_turret_model_index =
        mgr->RegisterModel(turret_vertices, turret_indices);
  }
}

void NeutralTurret::Render() {
  if (!setBlock_) {
    game_core_->AddObstacle<obstacle::Block>(position_);
    setBlock_ = true;
  }
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(neutral_turret_model_index);
}

void NeutralTurret::Update() {
  TurretRotate();
  Fire();
}

glm::highp_vec2 NeutralTurret::findEnermy() {
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
  if (glm::length(diff) < range_)
    return diff;
  else
    return position_ - position_;
}

void NeutralTurret::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    // auto &input_data = player->GetInputData();
    // auto diff = input_data.mouse_cursor_position - position_;
    auto diff = findEnermy();
    if (glm::length(diff) < 1e-4) {
      // turret_rotation_ = rotation_;
      fire_count_down_ = 1;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void NeutralTurret::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      // auto &input_data = player->GetInputData();
      // if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
      auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
      GenerateBullet<bullet::CannonBall>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
          GetDamageScale(), velocity);
      fire_count_down_ = kTickPerSecond * 2;  // Fire interval 2 second.
      // }
    }
  }
}

bool NeutralTurret::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -1.0f && position.x < 1.0f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 2.0f &&
         position.y - position.x < 2.0;
}

const char *NeutralTurret::UnitName() const {
  return "Neutral Turret";
}

const char *NeutralTurret::Author() const {
  return "whm and kfh";
}

}  // namespace battle_game::unit
