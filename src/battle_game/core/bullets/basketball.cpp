#include "battle_game/core/bullets/basketball.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
Basketball::Basketball(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale,
                       glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity){
}

void Basketball::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.4f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/basketball.png");
  DrawModel(0);
}

void Basketball::Update() {
  auto last_position_ = position_;
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    if (!rebounding_times_left_) 
    {
      should_die = true;
    } else {
      glm::vec2 tmp1 = glm::vec2(-1.0f, 1.0f);
      auto tmp_v = velocity_ * tmp1; 
      auto next_position = position_ + tmp_v * kSecondPerTick;
      if (game_core_->IsBlockedByObstacles(next_position)) {
        glm::vec2 tmp = glm::vec2(1.0f, -1.0f);
        velocity_ *= tmp;
      } else {
        velocity_ = tmp_v;
      }
      rebounding_times_left_--;
    }
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

Basketball::~Basketball() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
