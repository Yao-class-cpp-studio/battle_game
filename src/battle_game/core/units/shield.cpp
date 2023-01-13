#include "shield.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

Shield::Shield(GameCore *game_core,
               uint32_t id,
               uint32_t player_id,
               uint32_t lord_id)
    : Unit(game_core, id, player_id) {
  lord_id_ = lord_id;
}

void Shield::Render() {
  battle_game::SetTransformation(position_, 0.0f, scale_);
  battle_game::SetColor({0.0f, 0.72f, 0.93f, 1.0f});
  battle_game::SetTexture("../../textures/shield.png");
  DrawModel();
}

void Shield::Update() {
  ShieldMove();
  HealthDecrease();
}

bool Shield::IsHit(glm::vec2 position) const {
  auto diff = position - position_;
  return glm::length(diff) <= 2.0f;
}

void Shield::ShieldMove() {
  if (lord_id_) {
    auto *lord = game_core_->GetUnit(lord_id_);
    this->SetPosition(lord->GetPosition());
  }
}

void Shield::HealthDecrease() {
  if (health_ < 2e-3)
    game_core_->PushEventRemoveUnit(id_);
  else
    SetHealth(health_ - 2e-3);
}

const char *Shield::UnitName() const {
  return "Shield";
}

const char *Shield::Author() const {
  return "CoffeePot";
}

}  // namespace battle_game::unit
