#include "battle_game/core/particles/bullet_hole.h"

#include "battle_game/core/game_core.h"

namespace battle_game::particle {
BulletHole::BulletHole(GameCore *game_core,
                       uint32_t id,
                       glm::vec2 position,
                       float rotation,
                       uint32_t duration)
    : Particle(game_core, id, position, rotation), duration_(duration) {
}

void BulletHole::Render() {
  SetTransformation(position_, rotation_, glm::vec2{1.0f});
  SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
  SetTexture("../../textures/bullet_hole.png");
  DrawModel(0);
}

void BulletHole::Update() {
  duration_--;
  if (duration_ <= 0) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
