#pragma once
#include "battle_game/core/effect.h"

namespace battle_game {

class Invisible : public Effect {
 private:
  float health_per_second_;
  uint32_t tick_remain_;

 public:
  std::string Name() const {
    return u8"隐身";
  }
  std::string Description() const;
  void Influence(Status &status);
  uint32_t TickRemain() const;
  void TickPass();
  bool ShouldRemove() const;
  Invisible(uint32_t src_unit_id, uint32_t tick_remain);
};

}  // namespace battle_game