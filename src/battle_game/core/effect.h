#pragma once
#include "battle_game/core/unit.h"
#include "glm/glm.hpp"

#define PUSH_EFFECT(effect) PushEffect((Effect *)(new effect));

namespace battle_game {

class Effect {
 public:
  virtual std::string Name() const = 0;
  virtual std::string Description() const {
    return "";
  }
  virtual void Influence(Status &) {
  }
  virtual uint32_t TickRemain() const = 0;
  virtual void TickPass() = 0;
  virtual bool ShouldRemove() const = 0;
  Effect(uint32_t src_unit_id);

 protected:
  uint32_t src_unit_id_;
};

}  // namespace battle_game
