#include "battle_game/core/bullets/laser.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
Laser::Laser(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity) {
  rotation_ = std::atan(velocity_.y / velocity_.x);
}

void Laser::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.5f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/laser.png");
  DrawModel(0);
}

void Laser::Update() {
  fly_time_ -= kSecondPerTick;
  glm::vec2 old_position_ = position_;
  position_ += velocity_ * kSecondPerTick;
  if (game_core_->IsBlockedByObstacles(position_)) {
    if (!reflected_) {
      glm::vec2 unv = game_core_->GetUnitNormalVec2OfSurface(position_);
      float prod = unv.x * velocity_.x + unv.y * velocity_.y;
      velocity_ -= 2.0f * prod * unv;
      rotation_ = std::atan(velocity_.y / velocity_.x);
      reflected_ = true;
    } 
  } else if (reflected_) {
    reflected_ = false;  
  }
  if (game_core_->IsOutOfRange(position_)) {
    position_ = old_position_;
  }
  bool should_die = false;
  if (fly_time_ <= 0) {
    should_die = true;
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

Laser::~Laser() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
