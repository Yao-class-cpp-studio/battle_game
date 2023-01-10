#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/ring_segments_.h"

namespace battle_game::particle {
Ring_segments::Ring_segments(GameCore *game_core,
                             uint32_t id,
                             glm::vec2 position,
                             float rotation,
                             glm::vec2 v,
                             float size,
                             glm::vec4 color,
                             float decay_scale)
    : Particle(game_core, id, position, rotation),
      v_(v),
      size_(size),
      color_(color),
      decay_scale_(decay_scale) {
}
void Ring_segments::Render() {
  SetTransformation(position_, rotation_, glm::vec2{size_});
  SetColor(glm::vec4{glm::vec4{1.0f}} * color_);
  SetTexture("../../textures/particle2.png");
  DrawModel();
}
void Ring_segments::Update() {
  if (!decay_timer_) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
