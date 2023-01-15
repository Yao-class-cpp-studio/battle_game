#include "battle_game/core/game_core.h"

namespace battle_game {

Status::Status(GameCore *game_core,
               Unit *unit,
               std::function<bool(glm::vec2)> is_hit,
               float max_health,
               float health,
               float attack,
               float defence,
               float speed)
    : game_core_{game_core},
      unit_{unit},
      base_is_hit_{is_hit},
      max_health_{max_health},
      health_{health},
      base_attack_{attack},
      base_defence_{defence},
      base_speed_(speed) {
}

void Status::HealthChange(uint32_t src_unit_id, float change) {
  if ((health_ += change / max_health_) <= 0.0f)
    game_core_->PushEventKillUnit(unit_->GetId(), src_unit_id);
  if (health_ > 1.0f)
    health_ = 1.0f;
}

void Status::Initialization() {
  attack_ = base_attack_;
  defence_ = base_defence_;
  speed_ = base_speed_;
  is_hit_ = base_is_hit_;
}

}  // namespace battle_game