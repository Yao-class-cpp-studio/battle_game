#include "battle_game/core/particles/charge.h"

#include "battle_game/core/game_core.h"

namespace battle_game::particle {
Charge::Charge(GameCore *game_core,
               uint32_t id,
               glm::vec2 position,
               float rotation,
               glm::vec2 v,
               float size,
               glm::vec4 color,
               float decay_scale)
    : Smoke(game_core, id, position, rotation, v, size, color, decay_scale),
      v_(v),
      size_(size),
      color_(color),
      decay_scale_(decay_scale) {
}

void Charge::Render() {
  SetTransformation(position_, rotation_, glm::vec2{size_});
  SetColor();
  SetTexture("../../textures/charge_particle.png");
  DrawModel();
}

}  // namespace battle_game::particle
