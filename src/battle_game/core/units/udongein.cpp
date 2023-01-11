#include "udongein.h"
#include "spell_caster.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t model_index = 0xffffffffu;
}  // namespace

Udongein::Udongein(GameCore *game_core, uint32_t id, uint32_t player_id)
    : SpellCaster(game_core, id, player_id) {
  move_mode_ = Arrow;
  hitbox_radius_ = 0.5f;
  speed_ = 10.0f;
  }

void Udongein::Spell() {
  if (input_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
    DirectionalFire();
  else if (input_.mouse_button_down[GLFW_MOUSE_BUTTON_RIGHT])
    BoundaryBetweenWavesAndParticles();
}

void Udongein::DirectionalFire() {
  if (directional_fire.clock > 0)
    directional_fire.clock -= 10;
  else {
    Udongein::Fire<bullet::CannonBall>(
        getCursorDirection(input_.mouse_cursor_position), 20.0f);
    directional_fire.clock = kTickPerSecond;  // Fire interval 1 second.
  }
}

void Udongein::BoundaryBetweenWavesAndParticles() {
  const float angle_diff_ =
      glm::radians(360.0f) / boundary_between_waves_and_particles.line;
  if (boundary_between_waves_and_particles.clock > 0)
    boundary_between_waves_and_particles.clock -= 30;
  else {
    boundary_between_waves_and_particles.angle +=
        (boundary_between_waves_and_particles.angle_increment +=
         glm::radians(0.5f));
    for (uint32_t i = 0; i < boundary_between_waves_and_particles.line; i++)
      Udongein::Fire<bullet::CannonBall>(
          boundary_between_waves_and_particles.angle + i * angle_diff_, 5.0f);
    boundary_between_waves_and_particles.clock =
        kTickPerSecond;
  }
}
}  // namespace battle_game::unit