#include "battle_game/core/units/zb_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

zb_tank::zb_tank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void zb_tank::Render() {
  Tank::Render();
}

void zb_tank::Update() {
  TankMove(4.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void zb_tank::Fire() {
  Tank::Fire();
}

bool zb_tank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *zb_tank::UnitName() const {
  return "zb tank";
}

const char *zb_tank::Author() const {
  return "Borui Zhang";
}
}  // namespace battle_game::unit
