#include "battle_game/core/particles/target.h"

#include "battle_game/core/game_core.h"

namespace battle_game::particle {
Target::Target(GameCore *game_core,
               uint32_t id,
               glm::vec2 position,
               float rotation,
               glm::vec4 color)
    : Particle(game_core, id, position, rotation), color_(color) {
}

void Target::Render() {
  SetTransformation(position_, rotation_, glm::vec2{size_});
  SetColor(glm::vec4{glm::vec3{1.0f}, strength_} * color_);
  SetTexture("../../textures/target.png");
  DrawModel();
}

void Target::Update() {
  lifetime_ -= kSecondPerTick;
  strength_ = std::sin(lifetime_ * glm::pi<float>());
  if (lifetime_ < 0.0f) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
