#pragma once
#include "battle_game/core/particle.h"

namespace battle_game::particle {
class Smoke : public Particle {
 public:
  Smoke(GameCore *game_core,
        uint32_t id,
        glm::vec2 position,
        float rotation,
        glm::vec2 v,
        float size = 0.2f,
        glm::vec4 color = glm::vec4{1.0f},
        float decay_scale = 1.0f);
  void Render() override;
  void Update() override;

 private:
  glm::vec2 v_{};
  float strength_{1.0f};
  float size_{};
  float decay_scale_{};
  glm::vec4 color_{};
};
}  // namespace battle_game::particle
