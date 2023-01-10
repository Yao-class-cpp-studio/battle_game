#pragma once
#include "battle_game/core/object.h"

namespace battle_game {
enum ParticleType // use lowercase to avoid class name
{
    untracked_particle,
    smoke
    // add your particle here:
    
};

class Particle : public Object {
 public:
  Particle(GameCore *game_core,
           uint32_t id,
           glm::vec2 position,
           float rotation = 0.0f);
  [[nodiscard]] ParticleType GetParticleType() const {
    return particle_type_;
  }
 protected:
    ParticleType particle_type_{untracked_particle};
 private:
};
}  // namespace battle_game
