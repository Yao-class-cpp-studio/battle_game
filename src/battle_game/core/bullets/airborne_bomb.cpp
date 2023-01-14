#include "battle_game/core/bullets/airborne_bomb.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
AirborneBomb::AirborneBomb(GameCore *core,
                           uint32_t id,
                           uint32_t unit_id,
                           uint32_t player_id,
                           glm::vec2 position,
                           float rotation,
                           float damage_scale,
                           glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      duration(20) {
}
void AirborneBomb::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.0f});
  SetColor({0.0f, 0.0f, 0.0f, 0.0f});
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}
void AirborneBomb::Update() {
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (duration) {
    duration--;
  } else {
    should_die = true;
  }
  if (game_core_->IsBlockedByObstacles(position_)) {
    for (int i = 0; i < 5; i++) {
      game_core_->PushEventGenerateParticle<particle::Smoke>(
          position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
          glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
    }
  }
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == player_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
      for (int i = 0; i < 5; i++) {
        game_core_->PushEventGenerateParticle<particle::Smoke>(
            position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
            glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
      }
    }
  }
  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}
AirborneBomb::~AirborneBomb() = default;
}  // namespace battle_game::bullet
