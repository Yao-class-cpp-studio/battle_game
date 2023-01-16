#include "battle_game/core/bullets/COVID_ball.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
CovidBall::CovidBall(GameCore *core,
                     uint32_t id,
                     uint32_t unit_id,
                     uint32_t player_id,
                     glm::vec2 position,
                     float rotation,
                     float damage_scale,
                     glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_rate_(std::pow(glm::dot(velocity, velocity), 0.5f)),
      velocity_angle_(std::atan2(velocity.y, velocity.x)),
      gravity_(((battle_game::unit::CovidTank *)(core->GetUnit(unit_id)))
                   ->GetBulletGravity()) {
}

void CovidBall::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.3f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/COVID.png");
  DrawModel(0);
}

void CovidBall::Update() {
  auto &units = game_core_->GetUnits();
  glm::vec2 current_diff_pos = {0.0f, 0.0f};
  float current_dis_square = 0.0f;
  float min_dis_square = 100.0f;
  glm::vec2 min_dis_diff_pos = {0.0f, 0.0f};
  bool has_unit = false;
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    has_unit = true;
    current_diff_pos = unit.second->GetPosition() - position_;
    current_dis_square = glm::dot(current_diff_pos, current_diff_pos);
    if (current_dis_square < min_dis_square) {
      min_dis_square = current_dis_square;
      min_dis_diff_pos = current_diff_pos;
    }
  }
  position_ += glm::vec2{std::cos(velocity_angle_), std::sin(velocity_angle_)} *
               velocity_rate_ * kSecondPerTick;
  if (has_unit) {
    float min_dis_diff_angle =
        std::atan2(min_dis_diff_pos.y, min_dis_diff_pos.x);
    float angle_diff = min_dis_diff_angle - velocity_angle_;
    if (angle_diff < -glm::pi<float>())
      angle_diff += 2 * glm::pi<float>();
    if (angle_diff > glm::pi<float>())
      angle_diff -= 2 * glm::pi<float>();
    velocity_angle_ += gravity_ * angle_diff * kSecondPerTick / min_dis_square;
  }

  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_,
                                      damage_scale_ * basic_damage_);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

CovidBall::~CovidBall() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
