#include "regeneration.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

std::string Regeneration::Description() const {
  return (std::string) "持续回复生命值（每秒" +
         std::to_string(health_per_second_) + (std::string) "，剩余" +
         std::to_string((float)tick_remain_ / kTickPerSecond) +
         (std::string) "）";
}

void Regeneration::Influence(Unit::Status &status) {
  status.HealthChange(src_player_id_, health_per_second_ / kTickPerSecond);
}

uint32_t Regeneration::TickRemain() const {
  return tick_remain_;
}

void Regeneration::TickPass() {
  tick_remain_--;
}

bool Regeneration::ShouldRemove() const {
  return !tick_remain_;
}

Regeneration::Regeneration(uint32_t src_player_id,
                           float health_per_second,
                           uint32_t tick_remain)
    : Effect(src_player_id),
      health_per_second_{health_per_second},
      tick_remain_{tick_remain} {
}

}  // namespace battle_game