#include "battle_game/core/particles/explosion.h"

#include "battle_game/core/game_core.h"

namespace battle_game::particle {
Explosion::Explosion(GameCore *game_core,
                     uint32_t id,
                     glm::vec2 position,
                     float rotation,
                     uint32_t duration)
    : Particle(game_core, id, position, rotation), duration_(duration) {
}

void Explosion::Render() {
  SetTransformation(position_, rotation_, glm::vec2{2.0f});
  SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
  SetTexture("../../textures/explosion.png");
  DrawModel(0);
}

void Explosion::Update() {
  if (should_damage_) {
    auto &units = game_core_->GetUnits();
    for (auto &unit : units) {
      auto position = unit.second->GetPosition();
      if (IsInExplosion(position)) {
        game_core_->PushEventDealDamage(unit.first, id_, 10.0f);
      }
    }
    should_damage_ = false;
  }
  duration_--;
  if (duration_ <= 0) {
    game_core_->PushEventRemoveParticle(id_);
  }
}

bool Explosion::IsInExplosion(glm::vec2 position) {
  position = WorldToLocal(position);
  return position.x > -1.6f && position.x < 1.6f && position.y > -2.0f &&
         position.y < 2.0f && position.x + position.y < 3.2f &&
         position.y - position.x < 3.2f;
}
}  // namespace battle_game::particle
