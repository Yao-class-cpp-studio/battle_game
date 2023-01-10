#include "spell_caster.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t model_index = 0xffffffffu;
}  // namespace

SpellCaster::SpellCaster(GameCore *game_core,
      uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~model_index) {
    auto mgr = AssetsManager::GetInstance();
    model_index = mgr->RegisterModel(
      {{{-0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
      {0, 1, 2, 1, 2, 3});
  }
}

void SpellCaster::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(model_index);
}

void SpellCaster::Update() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    last_input_ = input_;
    input_ = player->GetInputData();
    Move();
    Spell();
  }
}

float SpellCaster::getCursorDirection() {
  auto diff = input_.mouse_cursor_position - position_;
  if (glm::length(diff) < 1e-4)
    return rotation_;
  else
    return std::atan2(diff.y, diff.x) - glm::radians(90.0f);
}

void SpellCaster::Move() {
  glm::vec2 offset{0.0f};
  const float oblique_scale = std::sqrt(2) / 2;
  auto key_to_act = input_to_act_.key_down;
  auto key_input = input_.key_down;
  auto last_key_input = last_input_.key_down; 
    
  switch (move_mode_) { 
  case Arrow:
    if (auto_move_) {
      if (key_input[GLFW_KEY_UP] != last_key_input[GLFW_KEY_UP] ||
          key_input[GLFW_KEY_LEFT] != last_key_input[GLFW_KEY_LEFT] ||
          key_input[GLFW_KEY_DOWN] != last_key_input[GLFW_KEY_DOWN] ||
          key_input[GLFW_KEY_RIGHT] != last_key_input[GLFW_KEY_RIGHT]) {
        if (key_input[GLFW_KEY_UP] == key_to_act[GLFW_KEY_UP] &&
            key_input[GLFW_KEY_LEFT] == key_to_act[GLFW_KEY_LEFT] &&
            key_input[GLFW_KEY_DOWN] == key_to_act[GLFW_KEY_DOWN] &&
            key_input[GLFW_KEY_RIGHT] == key_to_act[GLFW_KEY_RIGHT]) {
          if (!last_key_input[GLFW_KEY_UP] &&
              !last_key_input[GLFW_KEY_LEFT] &&
              !last_key_input[GLFW_KEY_DOWN] &&
              !last_key_input[GLFW_KEY_RIGHT])
            key_to_act[GLFW_KEY_UP] = key_to_act[GLFW_KEY_LEFT] =
                key_to_act[GLFW_KEY_DOWN] = key_to_act[GLFW_KEY_RIGHT] =
                    false;
        } else if (key_input[GLFW_KEY_UP] || key_input[GLFW_KEY_LEFT] ||
                   key_input[GLFW_KEY_DOWN] || key_input[GLFW_KEY_RIGHT])
          key_to_act[GLFW_KEY_UP] = key_input[GLFW_KEY_UP],
          key_to_act[GLFW_KEY_LEFT] = key_input[GLFW_KEY_LEFT],
          key_to_act[GLFW_KEY_DOWN] = key_input[GLFW_KEY_DOWN],
          key_to_act[GLFW_KEY_RIGHT] = key_input[GLFW_KEY_RIGHT];
      }
    } else
      key_to_act[GLFW_KEY_UP] = key_input[GLFW_KEY_UP],
      key_to_act[GLFW_KEY_LEFT] = key_input[GLFW_KEY_LEFT],
      key_to_act[GLFW_KEY_DOWN] = key_input[GLFW_KEY_DOWN],
      key_to_act[GLFW_KEY_RIGHT] = key_input[GLFW_KEY_RIGHT];

    if (key_to_act[GLFW_KEY_UP])
      offset.y += 1.0f;
    if (key_to_act[GLFW_KEY_LEFT])
      offset.x -= 1.0f;
    if (key_to_act[GLFW_KEY_DOWN])
      offset.y -= 1.0f;
    if (key_to_act[GLFW_KEY_RIGHT])
      offset.x += 1.0f;
    if (offset.x && offset.y)
      offset *= oblique_scale;
    break;
  case WASD:
    if (auto_move_) {
      if (key_input[GLFW_KEY_W] != last_key_input[GLFW_KEY_W] ||
          key_input[GLFW_KEY_A] != last_key_input[GLFW_KEY_A] ||
          key_input[GLFW_KEY_S] != last_key_input[GLFW_KEY_S] ||
          key_input[GLFW_KEY_D] != last_key_input[GLFW_KEY_D]) {
        if (key_input[GLFW_KEY_W] == key_to_act[GLFW_KEY_W] &&
            key_input[GLFW_KEY_A] == key_to_act[GLFW_KEY_A] &&
            key_input[GLFW_KEY_S] == key_to_act[GLFW_KEY_S] &&
            key_input[GLFW_KEY_D] == key_to_act[GLFW_KEY_D]) {
          if (!last_key_input[GLFW_KEY_W] &&
              !last_key_input[GLFW_KEY_A] &&
              !last_key_input[GLFW_KEY_S] &&
              !last_key_input[GLFW_KEY_D])
            key_to_act[GLFW_KEY_W] = key_to_act[GLFW_KEY_A] =
                key_to_act[GLFW_KEY_S] = key_to_act[GLFW_KEY_D] =
                    false;
        } else if (key_input[GLFW_KEY_W] || key_input[GLFW_KEY_A] ||
                    key_input[GLFW_KEY_S] || key_input[GLFW_KEY_D])
          key_to_act[GLFW_KEY_W] = key_input[GLFW_KEY_W],
          key_to_act[GLFW_KEY_A] = key_input[GLFW_KEY_A],
          key_to_act[GLFW_KEY_S] = key_input[GLFW_KEY_S],
          key_to_act[GLFW_KEY_D] = key_input[GLFW_KEY_D];
      }
    } else
      key_to_act[GLFW_KEY_W] = key_input[GLFW_KEY_W],
      key_to_act[GLFW_KEY_A] = key_input[GLFW_KEY_A],
      key_to_act[GLFW_KEY_S] = key_input[GLFW_KEY_S],
      key_to_act[GLFW_KEY_D] = key_input[GLFW_KEY_D];

    if (key_to_act[GLFW_KEY_W])
    offset.y += 1.0f;
    if (key_to_act[GLFW_KEY_A])
    offset.x -= 1.0f;
    if (key_to_act[GLFW_KEY_S])
    offset.y -= 1.0f;
    if (key_to_act[GLFW_KEY_D])
    offset.x += 1.0f;
    if (offset.x && offset.y)
      offset *= oblique_scale;
    break;
  case Mouse:
    offset = Rotate({0.0f, 1.0f}, getCursorDirection());
    break;
  }
  offset *= kSecondPerTick * speed_ * GetSpeedScale();
  auto new_position = position_ + offset;
  if (!game_core_->IsBlockedByObstacles(new_position))
    game_core_->PushEventMoveUnit(id_, new_position);
}

template <class BulletType>
void SpellCaster::Fire(float direction, float speed) {
  auto velocity = Rotate(glm::vec2{0.0f, speed}, direction);
  GenerateBullet<BulletType>
    (position_ + Rotate({0.0f, 1.2f}, direction), 
      direction, GetDamageScale(), velocity);
}

void SpellCaster::Spell() {}

Udongein::Udongein(GameCore *game_core, uint32_t id, uint32_t player_id)
    : SpellCaster(game_core, id, player_id) {}

bool Udongein::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x * position.x + position.y * position.y <=
         hitbox_radius_ * hitbox_radius_;
}

void Udongein::Spell() {
  if (input_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
    DirectionalFire();
  else if (input_.mouse_button_down[GLFW_MOUSE_BUTTON_RIGHT])
    BoundaryBetweenWavesAndParticles();
}

void Udongein::DirectionalFire() {
  if (directional_fire.clock_ > 0)
    directional_fire.clock_ -= 10;
  else {
    Fire<bullet::CannonBall>(getCursorDirection(), 20.0f);
    directional_fire.clock_ = kTickPerSecond;  // Fire interval 1 second.
  }
}

void Udongein::BoundaryBetweenWavesAndParticles() {
  int line = 5;
  const float angle_diff_ = glm::radians(360.0f) / line;
  if (boundary_between_waves_and_particles.clock_ > 0)
    boundary_between_waves_and_particles.clock_ -= 30;
  else {
    boundary_between_waves_and_particles.angle_ +=
        (boundary_between_waves_and_particles.angle_increment_ +=
         glm::radians(0.5f));
    while (line--)
    Fire<bullet::CannonBall>(
        boundary_between_waves_and_particles.angle_ + line * angle_diff_, 5.0f);
    boundary_between_waves_and_particles.clock_ =
        kTickPerSecond;  // Fire interval 1 second.
  }
}
}  // namespace battle_game::unit