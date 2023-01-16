#include "battle_game/core/units/zb_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"

namespace battle_game::unit {

ZbTank::ZbTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

/*void ZbTank::Render() {
  Tank::Render();
}

void ZbTank::Update() {
  TankMove(4.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void ZbTank::Fire() {
  Tank::Fire();
}

bool ZbTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}*/

const char *ZbTank::UnitName() const {
  return "zb tank";
}

const char *ZbTank::Author() const {
  return "Borui Zhang";
}
}  // namespace battle_game::unit
