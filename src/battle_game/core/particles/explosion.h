#pragma once
#include "battle_game/core/particle.h"

namespace battle_game::particle {
class Explosion : public Particle {
 public:
  Explosion(GameCore *game_core,
            uint32_t id,
            glm::vec2 position,
            float rotation,
            uint32_t duration);
  void Render() override;
  void Update() override;
  bool IsInExplosion(glm::vec2 position);

 private:
  uint32_t duration_{};
  bool should_damage_{true};
};
}  // namespace battle_game::particle
