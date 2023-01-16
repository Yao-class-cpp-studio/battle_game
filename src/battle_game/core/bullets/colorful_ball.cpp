#include "battle_game/core/bullets/colorful_ball.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
ColorfulBall::ColorfulBall(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale,
                       glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      count_down_(kTickPerSecond * 0.8) {
  color_ = {game_core_->RandomFloat(), game_core_->RandomFloat(),
            game_core_->RandomFloat(), 1.0f};
  auto total_velocity =
      std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y) / 2;
  auto velocity_rotation = std::atan2(velocity.y, velocity.x);
  velocity_rotation += 1 * (game_core_->RandomFloat() - 0.5);
  velocity_.x = total_velocity * std::cos(velocity_rotation);
  velocity_.y = total_velocity * std::sin(velocity_rotation);
}

void ColorfulBall::update_color(float &a) {
  a += 0.3 * (game_core_->RandomFloat() - 0.5);
  a = a > 0 ? a : 0;
  a = a < 1 ? a : 1;
}

void ColorfulBall::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  update_color(color_.r);
  update_color(color_.g);
  update_color(color_.b);
  SetColor(color_);
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}

void ColorfulBall::Update() {
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if ((game_core_->IsBlockedByObstacles(position_) 
      && !game_core_->IsStuckBySwamp(position_)) || --count_down_ == 0) {
    should_die = true;
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 5.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

ColorfulBall::~ColorfulBall() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
