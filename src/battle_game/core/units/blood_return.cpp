#include "battle_game/core/units/blood_return.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
namespace {
uint32_t blood_model_index = 0xffffffffu;
}

BloodReturn::BloodReturn(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~blood_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      blood_model_index = mgr->RegisterModel(
          {
              {{-0.6f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.6f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},

              {{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.6f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2, 3, 2, 3, 4, 1, 5, 2, 2, 4});
    }
  }
}

void BloodReturn::Update() {
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.second->IsHit(position_)) {
      unit.second->SetHealth(unit.second->GetHealth() + 0.3f);
      game_core_->PushEventRemoveUnit(id_);
    }
  }
}

void BloodReturn::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.6f, 0.0f, 0.4f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(blood_model_index);
}

bool BloodReturn::IsHit(glm::vec2 position) const {
  return false;
}

void BloodReturn::RenderLifeBar() {
}

const char *BloodReturn::UnitName() const {
  return "Blood Package";
}

const char *BloodReturn::Author() const {
  return "Whm and Kfh";
}

}  // namespace battle_game::unit
