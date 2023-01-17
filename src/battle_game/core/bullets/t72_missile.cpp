#include "battle_game/core/bullets/t72_missile.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
T72Missile::T72Missile(GameCore *core,
           uint32_t id,
           uint32_t unit_id,
           uint32_t player_id,
           glm::vec2 position,
           float rotation,
           float damage_scale,
           glm::vec2 velocity,
           uint32_t life_time)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      life_time_(life_time),
      total_time_(life_time) {
}

void T72Missile::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.4f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/t72_missile.png");
  DrawModel(0);
}

void T72Missile::Update() {
  bool should_die = false;
  if (life_time_) {
    life_time_--;
    position_ += velocity_ * kSecondPerTick;

    if (game_core_->IsBlockedByObstacles(position_)) {
      should_die = true;
    }

    auto &units = game_core_->GetUnits();
    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        continue;
      }
      if (unit.second->IsHit(position_)) {
        game_core_->PushEventDealDamage(
            unit.first, id_,
            damage_scale_ * 100.0f * (float)life_time_ / (float)total_time_);
        auto position = unit.second->GetPosition();
        game_core_->PushEventGenerateParticle<particle::Explosion>(position,
                                                                   0.0f, 30);
      }
    }
  } else {
    should_die = true;
  }
  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

T72Missile::~T72Missile() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
