#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

TankXxy::TankXxy(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

const char *TankXxy::UnitName() const {
  return "Tank Xxy";
}

const char *TankXxy::Author() const {
  return "Megumi X";
}
}  // namespace battle_game::unit
