#include "battle_game/core/units/PhantomOfCosin.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

PhantomOfCosin::PhantomOfCosin (GameCore *game_core,
                                   uint32_t id,
                                   uint32_t player_id)
    : Tank(game_core, id, player_id) {
}


 void PhantomOfCosin::Update() {
 
    TankMove(6.17f, glm::radians(180.0f));
    TurretRotate();
    Fire();
 
  
}





const char *PhantomOfCosin::UnitName() const {
  return "phantom_of_cosin";
}

const char *PhantomOfCosin::Author() const {
  return "csccsru";
}
}  // namespace battle_game::unit