#include "invisible.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

std::string Invisible::Description() const {
  return (std::string)u8"无法被子弹识别（剩余" +
         std::to_string((float)TickRemain() / kTickPerSecond) +
         (std::string)u8"）";
}

void Invisible::Influence(Status &status) {
  status.is_hit_ = [](glm::vec2) { return false; };
}

uint32_t Invisible::TickRemain() const {
  return tick_remain_;
}

void Invisible::TickPass() {
  tick_remain_--;
}

bool Invisible::ShouldRemove() const {
  return !tick_remain_;
}

Invisible::Invisible(uint32_t src_unit_id, uint32_t tick_remain)
    : Effect(src_unit_id), tick_remain_{tick_remain} {
}

}  // namespace battle_game