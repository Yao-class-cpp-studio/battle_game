#include "TankAngel.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t TankAngel_model_index = 0xffffffffu;
}

TankAngel::TankAngel(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  HideLifeBar();
  if (!~TankAngel_model_index) {
    auto mgr = AssetsManager::GetInstance();
    std::vector<ObjectVertex> angel_vertices;
    std::vector<uint32_t> angel_indices;
    const int precision = 60;
    const float inv_precision = 1.0f / float(precision);
    for (int i = 0; i < precision; ++i) {
      auto theta = (float(i) + 0.5f) * inv_precision * glm::pi<float>() * 2.0f;
      angel_vertices.push_back(
          {{std::cos(theta) * 0.25f, std::sin(theta) * 0.25f},
           {0.0f, 0.0f},
           {1.0f, 1.0f, 1.0f, 1.0f}});
      angel_indices.push_back(i);
      angel_indices.push_back((i + 1) % precision);
      angel_indices.push_back(precision);
    }
    angel_vertices.push_back(
        {{0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}});
    TankAngel_model_index = mgr->RegisterModel(angel_vertices, angel_indices);
  }
}

void TankAngel::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(TankAngel_model_index);
}

float TankAngel::BasicMaxHealth() const {
  return 1.0f;
}

void TankAngel::Update() {
  TankMove(3.0f, glm::radians(180.0f));
}

bool TankAngel::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return glm::length(position) < 0.24;
}

const char *TankAngel::UnitName() const {
  return "TankAngel";
}

const char *TankAngel::Author() const {
  return "Starusc";
}
}  // namespace battle_game::unit
