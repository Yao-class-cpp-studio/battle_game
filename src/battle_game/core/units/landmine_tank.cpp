#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"

namespace battle_game::unit {

LandmineTank::LandmineTank(GameCore *game_core,
                             uint32_t id,
                             uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

const char *LandmineTank::UnitName() const {
  return "Landmine Tank";
}

const char *LandmineTank::Author() const {
  return "tzs";
}
}  // namespace battle_game::unit
