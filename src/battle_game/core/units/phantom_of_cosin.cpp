#include "battle_game/core/units/phantom_of_cosin.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

phantom_of_cosin::phantom_of_cosin(GameCore *game_core,
                                   uint32_t id,
                                   uint32_t player_id)
    : Tank(game_core, id, player_id) {
}


void phantom_of_cosin::Update() {
 
    TankMove(6.17f, glm::radians(180.0f));
    TurretRotate();
    Fire();
 
  
}





const char *phantom_of_cosin::UnitName() const {
  return "phantom_of_cosin";
}

const char *phantom_of_cosin::Author() const {
  return "csccsru";
}
}  // namespace battle_game::unit