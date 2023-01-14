#include "battle_game/core/particles/thunderbolt.h"

#include "battle_game/core/game_core.h"

namespace battle_game::particle {
Thunderbolt::Thunderbolt(GameCore *game_core,
                         uint32_t id,
                         glm::vec2 position,
                         float rotation,
                         uint32_t duration)
    : Particle(game_core, id, position, rotation), duration_(duration) {
}

void Thunderbolt::Render() {
  SetTransformation(position_, rotation_, glm::vec2{1.5f});
  SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
  SetTexture("../../textures/thunderbolt.png");
  DrawModel(0);
}

void Thunderbolt::Update() {
  duration_--;
  if (duration_ <= 0) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
