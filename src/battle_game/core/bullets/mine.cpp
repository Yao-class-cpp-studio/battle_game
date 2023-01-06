#include "battle_game/core/bullets/mine.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
  Mine::Mine(GameCore* core,
    uint32_t id,
    uint32_t unit_id,
    uint32_t player_id,
    glm::vec2 position,
    float rotation,
    float damage_scale,
    uint32_t velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
    velocity_(0), ready_count_down_(2 * kTickPerSecond) {
  }

  void Mine::Render() {
    SetTransformation(position_, rotation_, glm::vec2{ 1.0f });
    SetColor(game_core_->GetPlayerColor(player_id_));
    SetTexture("../../textures/mine.png");
    DrawModel(0);
  }

  void Mine::Update() {
    if (ready_count_down_) {
      ready_count_down_--;
    }
    else {
      bool should_die = false;

      auto& units = game_core_->GetUnits();
      for (auto& unit : units) {
        if (unit.first == player_id_) {
          continue;
        }
        if (unit.second->IsHit(position_)) {
          game_core_->PushEventDealDamage(
            unit.first, id_, game_core_->GetUnit(unit.first)->GetMaxHealth());
          should_die = true;
        }
      }

      if (should_die) {
        game_core_->PushEventRemoveBullet(id_);
      }
    }
  }

  Mine::~Mine() {
    for (int i = 0; i < 5; i++) {
      game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 5.0f, 0.2f,
        glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }, 3.0f);
    }
  }
}  // namespace battle_game::bullet
