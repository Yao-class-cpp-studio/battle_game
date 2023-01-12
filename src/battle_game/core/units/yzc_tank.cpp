#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "battle_game/core/units/yzc_tank.h"
#include "tiny_tank.h"
namespace battle_game::unit {
Tankyzc::Tankyzc(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id){};

const char *Tankyzc::UnitName() const {
  return "Tankyzc";
}

const char *Tankyzc::Author() const {
  return "Yzc";
}
}  // namespace battle_game::unit