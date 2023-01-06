#pragma once
#include "battle_game/core/object.h"

namespace battle_game {
class Particle : public Object {
 public:
  Particle(GameCore *game_core,
           uint32_t id,
           glm::vec2 position,
           float rotation = 0.0f);

 private:
};
}  // namespace battle_game
