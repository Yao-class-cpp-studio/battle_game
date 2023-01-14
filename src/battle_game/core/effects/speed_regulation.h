#pragma once
#include "battle_game/core/effect.h"

namespace battle_game {

class SpeedRegulation : public Effect {
 public:
  std::string Name() const {
    return "速度调节";
  }
  std::string Description() const {
    return "单位速度受到与光标距离调节（单位特色效果）";
  }
  void Influence(Unit::Status &status);
  uint32_t TickRemain() const;
  void TickPass();
  bool ShouldRemove() const;
  SpeedRegulation(uint32_t src_unit_id);
};

}  // namespace battle_game