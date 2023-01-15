#include "energy_beam.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
namespace battle_game::bullet {

EnergyBeam::EnergyBeam(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale) {
}
EnergyBeam::~EnergyBeam() {
}

void EnergyBeam::Render() {
  auto [_type, _unit, target] = Target();
  auto midpoint = (position_ + target) / 2.0f;
  auto length = glm::length(target - position_);
  SetTransformation(midpoint, rotation_, glm::vec2{0.1f, length / 2});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture(0);
  DrawModel(0);
}

void EnergyBeam::Update() {
  auto [type, unit, target] = Target();
  if (type == HitResultType::Unit) {
    game_core_->PushEventDealDamage(unit->GetId(), id_,
                                    damage_scale_ * 10.0f * kSecondPerTick);
  }
  if (type != HitResultType::Miss && game_core_->RandomFloat() < 0.2) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        target, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        game_core_->GetPlayerColor(player_id_), 3.0f);
  }
  game_core_->PushEventRemoveBullet(id_);
}
EnergyBeam::HitResult EnergyBeam::Target() const {
  auto current = position_;
  const auto step_delta = Rotate({0.0f, 0.1f}, rotation_);
  const auto &units = game_core_->GetUnits();
  for (int step = 0; step < 100; step++) {
    if (game_core_->IsBlockedByObstacles(current)) {
      return {HitResultType::Obstacle, nullptr, current};
    }
    for (const auto &[id, unit] : units) {
      if (id != unit_id_ && unit->IsHit(current)) {
        return {HitResultType::Unit, &*unit, current};
      }
    }
    current += step_delta;
  }
  return {HitResultType::Miss, nullptr, current};
}
}  // namespace battle_game::bullet
