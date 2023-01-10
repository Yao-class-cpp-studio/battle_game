#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/bullet_storage_.h"

namespace battle_game::particle {
Bullet_storage::Bullet_storage(GameCore *game_core,
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
void Bullet_storage::Render() {
  SetTransformation(position_, rotation_, glm::vec2{size_});
  SetColor(glm::vec4{glm::vec4{1.0f}} * color_);
  SetTexture("../../textures/particle3.png");
  DrawModel();
}
void Bullet_storage::Update() {
  if (!decay_timer_) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
