#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/expansion_.h"

namespace battle_game::particle {
Expansion::Expansion(GameCore *game_core,
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
      decay_scale_(decay_scale),
      lapse_(140) {
}
void Expansion::Render() {
  SetTransformation(position_, rotation_, glm::vec2{size_});
  SetColor(glm::vec4{glm::vec4{1.0f}} * color_);
  SetTexture("../../textures/particle2.png");
  DrawModel();
}
void Expansion::Update() {
  if (lapse_) {
    if (lapse_ > 120 && lapse_ <= 140) {
      game_core_->PushEventGenerateParticle<particle::RingSegments>(
          position_, rotation_, glm::vec2{0.0f, 0.0f},
          11.7 * float(140 - lapse_) / 60, glm::vec4{1.0f, 0.5f, 0.0f, 0.8f},
          3.0f);
    } else {
      game_core_->PushEventGenerateParticle<particle::RingSegments>(
          position_, rotation_, glm::vec2{0.0f, 0.0f},
          float(11) / 3 + float(120 - lapse_) * 2 / 60,
          glm::vec4{1.0f, 0.5f, 0.0f, 0.8 - float(120 - lapse_) / 150}, 3.0f);
    }
    lapse_--;
  }
  if (!lapse_) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
