#include "battle_game/core/units/tank_han_wang.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"
// Format.

namespace battle_game::unit {

TankHanWang::TankHanWang(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

const char *TankHanWang::UnitName() const {
  return "Tank Han Wang";
}

const char *TankHanWang::Author() const {
  return "Han Wang";
}

}  // namespace battle_game::unit
