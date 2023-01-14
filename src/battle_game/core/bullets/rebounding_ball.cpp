#include "battle_game/core/bullets/rebounding_ball.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
ReboundingBall::ReboundingBall(GameCore *core,
                               uint32_t id,
                               uint32_t unit_id,
                               uint32_t player_id,
                               glm::vec2 position,
                               float rotation,
                               float damage_scale,
                               glm::vec2 velocity,
                               uint32_t rebounding_times)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      rebounding_times_left_(rebounding_times) {
}

void ReboundingBall::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/rebounding_ball.png");
  DrawModel(0);
}

float ReboundingBall::vec2_dot(glm::vec2 a, glm::vec2 b) {
  return a.x * b.x + a.y * b.y;
}

void ReboundingBall::Update() {
  bool should_die = false;
  glm::vec2 last_position = position_;
  position_ += velocity_ * kSecondPerTick;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
    auto blocked_obstacle = game_core_->GetBlockedObstacle(position_);
    if (blocked_obstacle != nullptr && rebounding_times_left_) {
      auto normal =
          blocked_obstacle->GetSurfaceNormal(last_position, position_);
      if (normal.second != glm::vec2(0, 0)) {
        rebounding_times_left_--;
        position_ =
            position_ - vec2_dot(normal.second, position_ - normal.first) *
                            normal.second * 2.0f;
        velocity_ = velocity_ -
                    vec2_dot(normal.second, velocity_) * normal.second * 2.0f;
        should_die = false;
      }
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

ReboundingBall::~ReboundingBall() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
