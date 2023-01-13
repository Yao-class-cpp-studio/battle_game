#include "battle_game/core/obstacle.h"

#include "battle_game/core/game_core.h"
namespace battle_game {
namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

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
