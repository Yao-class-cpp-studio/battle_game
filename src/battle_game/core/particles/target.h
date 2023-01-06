#pragma once
#include "battle_game/core/particle.h"

namespace battle_game::particle {
class Target : public Particle {
 public:
  Target(GameCore *game_core,
        uint32_t id,
        glm::vec2 position,
        float rotation,
        glm::vec4 color = glm::vec4{1.0f});
  void Render() override;
  void Update() override;

 private:
  float strength_;
  float size_{1.0f};
  glm::vec4 color_{};
  float lifetime_{2.0f};
};
}  // namespace battle_game::particle
