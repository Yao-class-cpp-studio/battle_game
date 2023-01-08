#include "battle_game/core/obstacle.h"

namespace battle_game {

Obstacle::Obstacle(GameCore *game_core,
                   uint32_t id,
                   glm::vec2 position,
                   float rotation)
    : Object(game_core, id, position, rotation) {
}

void Obstacle::Update() {
}

void Obstacle::Render() {
}
}  // namespace battle_game
