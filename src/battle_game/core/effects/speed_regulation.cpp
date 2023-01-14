#include "speed_regulation.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

void SpeedRegulation::Influence(Unit::Status &status) {
  auto player = status.game_core_->GetPlayer(status.unit_->GetPlayerId());
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - status.unit_->GetPosition();
    status.speed_ *= std::min(glm::length(diff) / 15, 1.0f);
  } else
    status.speed_ *= status.game_core_->RandomFloat();
}

uint32_t SpeedRegulation::TickRemain() const {
  return 0;
}
void SpeedRegulation::TickPass() {
}
bool SpeedRegulation::ShouldRemove() const {
  return false;
}

SpeedRegulation::SpeedRegulation(uint32_t src_unit_id) : Effect(src_unit_id) {
}

}  // namespace battle_game