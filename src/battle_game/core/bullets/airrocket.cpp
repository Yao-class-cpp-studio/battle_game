#include "battle_game/core/bullets/airrocket.h"
#include "battle_game/core/game_core.h"

namespace battle_game::bullet {
AirRocket::AirRocket(GameCore *core,
                        uint32_t id,
                        uint32_t unit_id,
                        uint32_t player_id,
                        glm::vec2 position,
                       float rotation,
                       float damage_scale)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),lifetime_{kTickPerSecond}{
}

void AirRocket::Render() {
  if(lifetime_>=kTickPerSecond-2){
    for (int i = 0; i < 3; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
    }
  }
  else{
  if(hit_==true)
  game_core_->PushEventGenerateParticle<particle::Smoke>(
    position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
    glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}

void AirRocket::Update() {
  bool should_die = false;
  lifetime_--;
  if (lifetime_<=0) {
    should_die = true;
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == player_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      if (hit_==false)
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 5.0f);
      hit_=true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}
}  // namespace battle_game::bullet