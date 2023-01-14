#pragma once
#include "battle_game/core/effect.h"

namespace battle_game {

class Regeneration : public Effect {
 private:
  float health_per_second_;
  uint32_t tick_remain_;

 public:
  std::string Name() const {
    return "生命回复";
  }
  std::string Description() const;
  void Influence(Unit::Status &status);
  uint32_t TickRemain() const;
  void TickPass();
  bool ShouldRemove() const;
  Regeneration(uint32_t src_player_id,
               float health_per_second,
               uint32_t tick_remain);
};

}  // namespace battle_game