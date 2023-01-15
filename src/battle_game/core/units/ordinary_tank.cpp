#include "battle_game/core/units/ordinary_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

OrdinaryTank::OrdinaryTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void OrdinaryTank::Render() {
  Tank::Render();
}

void OrdinaryTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void OrdinaryTank::Fire() {
  Tank::Fire();
}

bool OrdinaryTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *OrdinaryTank::UnitName() const {
  return "OrdinaryTank";
}

const char *OrdinaryTank::Author() const {
  return "Ordinary";
}
}  // namespace battle_game::unit