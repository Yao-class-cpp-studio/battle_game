#include "replicator.h"

#include <math.h>

#include <iostream>
#include <vector>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

using namespace std;

const glm::vec2 selectable_position_rep[3] = {{0.0f, 6.0f},
                                              {7.0f, 7.0f},
                                              {-7.0f, 8.0f}};
const glm::vec4 selectable_color_rep[3] = {{0.3f, 0.0f, 0.3f, 1.0f},
                                           {0.0f, 0.3f, 0.0f, 1.0f},
                                           {0.0f, 0.0f, 0.3f, 1.0f}};
const glm::vec4 selectable_color_rep_bright[3] = {{0.8f, 0.0f, 0.8f, 1.0f},
                                                  {0.0f, 0.8f, 0.0f, 1.0f},
                                                  {0.0f, 0.0f, 0.8f, 1.0f}};
bool used_rep[3];

namespace battle_game::unit {
namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

const float sq = (float)sqrt(3.0) * 0.5;

Replicator::Replicator(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();

    vector<ObjectVertex> body_vertices;
    vector<uint32_t> body_indices;
    body_vertices.push_back(
        {{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{sq, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{sq, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{0.0, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{-sq, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{-sq, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_indices.push_back(0);
    body_indices.push_back(1);
    body_indices.push_back(2);
    body_indices.push_back(0);
    body_indices.push_back(2);
    body_indices.push_back(3);
    body_indices.push_back(0);
    body_indices.push_back(3);
    body_indices.push_back(4);
    body_indices.push_back(0);
    body_indices.push_back(4);
    body_indices.push_back(5);

    tank_body_model_index = mgr->RegisterModel(body_vertices, body_indices);

    std::vector<ObjectVertex> turret_vertices;
    std::vector<uint32_t> turret_indices;
    const int precision = 60;
    const float inv_precision = 1.0f / float(precision);
    for (int i = 0; i < precision; i++) {
      auto theta = (float(i) + 0.5f) * inv_precision;
      theta *= glm::pi<float>() * 2.0f;
      auto sin_theta = std::sin(theta);
      auto cos_theta = std::cos(theta);
      turret_vertices.push_back({{sin_theta * 0.35f, cos_theta * 0.35f},
                                 {0.0f, 0.0f},
                                 {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(i);
      turret_indices.push_back((i + 1) % precision);
      turret_indices.push_back(precision);
    }
    turret_vertices.push_back(
        {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});

    const int direction = 6;
    const float inv_direction = 1.0f / float(direction);
    const float pi = acos(-1);
    for (int i = 0; i < direction; ++i) {
      auto theta = 2 * pi * i * inv_direction;
      glm::vec2 turret_init[4];
      turret_init[0] = {0.1f * cos(theta + pi), 0.1f * sin(theta + pi)};
      turret_init[1] = {0.1f * cos(theta), 0.1f * sin(theta)};
      turret_init[2] = {1.2f * cos(theta + 0.5 * pi),
                        1.2f * sin(theta + 0.5 * pi)};
      turret_init[3] = turret_init[2];
      turret_init[2].x += turret_init[1].x,
          turret_init[2].y += turret_init[1].y;
      turret_init[3].x += turret_init[0].x,
          turret_init[3].y += turret_init[0].y;
      for (int j = 0; j < 4; ++j)
        turret_vertices.push_back(
            {turret_init[j], {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(60 + i * 4 + 1);
      turret_indices.push_back(60 + i * 4 + 2);
      turret_indices.push_back(60 + i * 4 + 3);
      turret_indices.push_back(60 + i * 4 + 2);
      turret_indices.push_back(60 + i * 4 + 3);
      turret_indices.push_back(60 + i * 4 + 4);
    }
    tank_turret_model_index =
        mgr->RegisterModel(turret_vertices, turret_indices);
  }
  ishit_count_ = 0;
  player_id_ = 244899;

  int nw_id = game_core_->RandomInt(0, 2);
  while (used_rep[nw_id]) {
    nw_id = game_core_->RandomInt(0, 2);
  }
  type_id_ = nw_id;
  used_rep[type_id_] = true;
  position_ = selectable_position_rep[type_id_];
  obstacle_id_ = game_core_->AddObstacle<obstacle::RepliBlock>(position_);
}

void Replicator::Render() {
  position_ = selectable_position_rep[type_id_];
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  if (ishit_count_)
    battle_game::SetColor(selectable_color_rep_bright[type_id_]);
  else
    battle_game::SetColor(selectable_color_rep[type_id_]);
  battle_game::DrawModel(tank_body_model_index);
  battle_game::DrawModel(tank_turret_model_index);
  if (ishit_count_)
    --ishit_count_;
}

void Replicator::Update() {
  if (ishit_count_ == 10)
    Fire();
}

void Replicator::Fire() {
  const int direction = 6;
  const float pi = acos(-1);
  for (int i = 0; i < direction; ++i) {
    float theta = 2.0f * pi * i / 6;
    if (theta > pi / 2)
      theta -= 2 * pi;
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, theta);
    GenerateBullet<bullet::CannonBall>(position_ + Rotate({0.0f, 1.2f}, theta),
                                       theta, GetDamageScale(), velocity);
  }
}

Replicator::~Replicator() {
  game_core_->PushEventRemoveObstacle(obstacle_id_);
  game_core_->NewReplicator();
  used_rep[type_id_] = false;
}

void Replicator::IsHit_Modify() {
  ishit_count_ = 10;
}

bool Replicator::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  position.x = fabs(position.x);
  position.y = fabs(position.y);
  if (position.x > sq)
    return false;
  float offset_amount = 1.0f - (float)sqrt(3.0) / 3 * position.x;
  if (position.y <= offset_amount) {
    const_cast<Replicator &>(*this).IsHit_Modify();
    return true;
  }
  return false;
}

const char *Replicator::UnitName() const {
  return "Replicator";
}

const char *Replicator::Author() const {
  return "Teddy";
}
}  // namespace battle_game::unit
