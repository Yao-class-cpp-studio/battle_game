#include "tiny_tank_xwx.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

Tank_xwx::Tank_xwx(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void Tank_xwx::Render() {
  Tank::Render();
}

void Tank_xwx::Update() {
  Tank::Update();
}

void Tank_xwx::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 30.0f}, turret_rotation_);
        GenerateBullet<bullet::BigCannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = 90;  // Fire interval 1 second.
      }
    }
  }
}

bool Tank_xwx::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}
const char *Tank_xwx::UnitName() const {
  return "A Tank";
}

const char *Tank_xwx::Author() const {
  return "xwx";
}
}  // namespace battle_game::unit
