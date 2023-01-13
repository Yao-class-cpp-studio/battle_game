#include "battle_game/core/units/Whaooooo_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

WhaoooooTank::WhaoooooTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void WhaoooooTank::Render() {
  Tank::Render();
}

void WhaoooooTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void WhaoooooTank::Fire() {
  Tank::Fire();
}

bool WhaoooooTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *WhaoooooTank::UnitName() const {
  return "Whaooooo Tank";
}

const char *WhaoooooTank::Author() const {
  return "Whaooooo";
}
}  // namespace battle_game::unit
