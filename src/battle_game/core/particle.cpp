#include "battle_game/core/particle.h"

namespace battle_game {
Particle::Particle(GameCore *game_core,
                   uint32_t id,
                   glm::vec2 position,
                   float rotation)
    : Object(game_core, id, position, rotation) {
}
}  // namespace battle_game
