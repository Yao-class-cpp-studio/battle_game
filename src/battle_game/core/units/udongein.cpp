#include "udongein.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "spell_caster.h"

namespace battle_game::unit {

namespace {
uint32_t model_index = 0xffffffffu;
uint32_t hitbox_model_index = 0xffffffffu;
}  // namespace

Udongein::Udongein(GameCore *game_core, uint32_t id, uint32_t player_id)
    : SpellCaster(game_core, id, player_id) {
  move_mode_ = Arrow;
  hitbox_radius_ = 0.5f;
  speed_ = 10.0f;
}

void Udongein::Render() {
  battle_game::SetTransformation(position_, 0.0f,
                                 glm::vec2{1.0f, 114.0f / 80.0f});
  battle_game::SetTexture("../../textures/udongein_model.png");
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(0);
}

void Udongein::Spell() {
  if (input_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
    FocusedFire();
  if (input_.key_down[GLFW_KEY_Z])
    DirectionalFire();
  if (input_.key_down[GLFW_KEY_X])
    BoundaryBetweenWavesAndParticles();
}

void Udongein::DirectionalFire() {
  if (is_moving_) {
    auto rotate = Rotate({0.0f, 1.0f}, directional_fire.direction_) -
                  Rotate({0.0f, 1.0f}, move_direction_);
    directional_fire.direction_ =
        std::atan2(rotate.y, rotate.x) - glm::radians(90.0f);
  }
  if (focused_fire.clock > 0)
    focused_fire.clock -= 10;
  else {
    Udongein::Fire<bullet::UdongeinDirectionalBullet>(
        directional_fire.direction_, 20.0f);
    focused_fire.clock = kTickPerSecond;
  }
}

void Udongein::FocusedFire() {
  if (focused_fire.clock > 0)
    focused_fire.clock -= 10;
  else {
    Fire<bullet::CannonBall>(getCursorDirection(input_.mouse_cursor_position),
                             20.0f);
    focused_fire.clock = kTickPerSecond;
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
      Fire<bullet::CannonBall>(
          boundary_between_waves_and_particles.angle + i * angle_diff_, 5.0f);
    boundary_between_waves_and_particles.clock = kTickPerSecond;
  }
}
}  // namespace battle_game::unit
