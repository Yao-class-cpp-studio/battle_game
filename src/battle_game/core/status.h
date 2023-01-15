#pragma once

#include "battle_game/core/unit.h"
#include "glm/glm.hpp"

namespace battle_game {

class Unit;

class Status {
 public:
  GameCore *const game_core_;
  Unit *const unit_;
  float attack_;
  float defence_;
  float speed_;
  std::function<bool(glm::vec2)> is_hit_;
  glm::vec2 color_;

  float GetHealth() const {
    return health_;
  }
  float GetMaxHealth() const {
    return max_health_;
  }

  Status(GameCore *game_core,
         Unit *unit,
         std::function<bool(glm::vec2)> is_hit,
         float max_health,
         float health,
         float attack,
         float defence,
         float speed);

  void Initialization();
  void HealthChange(uint32_t, float);

 private:
  float max_health_;
  float health_;
  const float base_attack_;
  const float base_defence_;
  const float base_speed_;
  const std::function<bool(glm::vec2)> base_is_hit_;
};

}  // namespace battle_game
