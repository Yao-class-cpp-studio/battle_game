#include "hidden_treasure.h"

#include <math.h>

#include <iostream>
#include <vector>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

using namespace std;
const glm::vec2 selectable_position[5] = {{-6.0f, 5.0f},
                                          {0.0f, -6.0f},
                                          {4.0f, -4.0f},
                                          {-6.0f, 0.0f},
                                          {3.0f, -7.0f}};
const glm::vec4 selectable_color[5] = {{0.7f, 0.0f, 0.8f, 1.0f},
                                       {0.7f, 0.8f, 0.0f, 1.0f},
                                       {0.0f, 0.8f, 0.7f, 1.0f},
                                       {0.5f, 0.5f, 0.5f, 1.0f},
                                       {0.0f, 0.0f, 0.8f, 1.0f}};
bool used[5];

namespace battle_game::unit {
namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

HiddenTreasure::HiddenTreasure(GameCore *game_core,
                               uint32_t id,
                               uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();

    vector<ObjectVertex> body_vertices;
    vector<uint32_t> body_indices;
    body_vertices.push_back(
        {{-1.05f, 1.05f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{-1.05f, -1.05f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{1.05f, 1.05f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_vertices.push_back(
        {{1.05f, -1.05f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    body_indices.push_back(0);
    body_indices.push_back(1);
    body_indices.push_back(2);
    body_indices.push_back(1);
    body_indices.push_back(2);
    body_indices.push_back(3);

    tank_body_model_index = mgr->RegisterModel(body_vertices, body_indices);
  }
  int nw_id = game_core_->RandomInt(0, 4);
  while (used[nw_id]) {
    nw_id = game_core_->RandomInt(0, 4);
  }
  type_id = nw_id;
  used[type_id] = true;
  position_ = selectable_position[type_id];
  obstacle_id = game_core_->AddObstacle<obstacle::Block>(position_);
  health_ = 0.1f * (float)game_core_->RandomInt(0, 10);
}

void HiddenTreasure::Render() {
  position_ = selectable_position[type_id];
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(selectable_color[type_id]);
  battle_game::DrawModel(tank_body_model_index);
}

void HiddenTreasure::Update() {
}

HiddenTreasure::~HiddenTreasure() {
  game_core_->PushEventRemoveObstacle(obstacle_id);
  game_core_->NewTreasure();
  used[type_id] = false;
}

bool HiddenTreasure::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -1.05f && position.x < 1.05f && position.y > -1.05f &&
         position.y < 1.05f;
}

const char *HiddenTreasure::UnitName() const {
  return "Hidden Treasure";
}

const char *HiddenTreasure::Author() const {
  return "Teddy";
}
}  // namespace battle_game::unit
