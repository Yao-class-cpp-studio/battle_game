#include "speed_regulation.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

void SpeedRegulation::Influence(Status &status) {
  auto player = status.game_core_->GetPlayer(status.unit_->GetPlayerId());
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - status.unit_->GetPosition();
    status.speed_ *= std::min(glm::length(diff) / 15, 1.0f);
  } else
    status.speed_ *= status.game_core_->RandomFloat();
}

uint32_t SpeedRegulation::TickRemain() const {
  return tick_remain_;
}
void SpeedRegulation::TickPass() {
  if (finity_)
    tick_remain_--;
}
bool SpeedRegulation::ShouldRemove() const {
  return finity_ ? !TickRemain() : false;
}

SpeedRegulation::SpeedRegulation(uint32_t src_unit_id,
                                 bool finity,
                                 uint32_t tick_remain)
    : Effect(src_unit_id), finity_{finity}, tick_remain_{tick_remain} {
}

}  // namespace battle_game