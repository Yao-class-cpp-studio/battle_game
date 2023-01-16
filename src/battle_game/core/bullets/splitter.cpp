#include "splitter.h"

#include "battle_game/core/bullets/splitter.h"
#include "battle_game/core/game_core.h"

namespace battle_game::bullet {
Splitter::Splitter(GameCore *core,
                   uint32_t id,
                   uint32_t unit_id,
                   uint32_t player_id,
                   glm::vec2 position,
                   float rotation,
                   float damage_scale,
                   glm::vec2 velocity,
                   uint32_t split_num)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      split_num_(split_num) {
}

void Splitter::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.2f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}

void Splitter::Update() {
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 20.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

Splitter::~Splitter() {
  if (game_core_) {
    float inv_split_num = 1.0f / float(split_num_);
    for (int i = 0; i < split_num_; i++) {
      auto theta = (float(i) + 0.5f) * inv_split_num;
      theta *= glm::pi<float>() * 2.0f;
      auto sin_theta = std::sin(theta);
      auto cos_theta = std::cos(theta);
      game_core_->PushEventGenerateBullet<bullet::CannonBall>(
          unit_id_, player_id_, position_ + Rotate({0.0f, 0.1f}, theta), theta,
          damage_scale_, Rotate({0.0f, glm::length(velocity_)}, theta));
    }
  }
}  // namespace battle_game::bullet
}  // namespace battle_game::bullet
