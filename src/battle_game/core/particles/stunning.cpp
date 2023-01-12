#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/stunning.h"

namespace battle_game::particle {
Stunning::Stunning(GameCore *game_core,
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

void Stunning::Render() {
  SetTransformation(position_, rotation_, glm::vec2{size_});
  SetColor(glm::vec4{glm::vec3{1.0f}, strength_} * color_);
  SetTexture("../../textures/particle_stunning_bullet.png");
  DrawModel();
}

void Stunning::Update() {
  position_ += v_ * kSecondPerTick;
  strength_ -= kSecondPerTick * decay_scale_;
  if (strength_ < 0.0f) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
