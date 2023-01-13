#pragma once
#include "battle_game/core/particle.h"

namespace battle_game::particle {
class Thunderbolt : public Particle {
 public:
  Thunderbolt(GameCore *game_core,
              uint32_t id,
              glm::vec2 position,
              float rotation,
              uint32_t duration);
  void Render() override;
  void Update() override;

 private:
  uint32_t duration_{};
};
}  // namespace battle_game::particle
