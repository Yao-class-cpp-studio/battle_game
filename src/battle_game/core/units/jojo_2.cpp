#include "battle_game/core/units/jojo_2.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

TwoWayJet::TwoWayJet(GameCore *game_core, uint32_t id, uint32_t player_id)
    : JoJo(game_core, id, player_id) {
}

void TwoWayJet::Render() {
  JoJo::Render();
}

void TwoWayJet::Update() {
  JoJoMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void TwoWayJet::ShootFowardandBackward() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    for (int i = 0; i <= 1; ++i) {
      auto offset = glm::radians(180.0f * i);
      velocity = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_ + offset);
      GenerateBullet<bullet::Chicken>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
          turret_rotation_ + offset, GetDamageScale(), velocity);
    }
  }
}

void TwoWayJet::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        float isBackward = game_core_->RandomFloat();
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        if (isBackward < 0.5f)
          ShootFowardandBackward();
        else {
          GenerateBullet<bullet::Chicken>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool TwoWayJet::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -1.0f && position.x < 1.0f && position.y > -1.4f &&
         position.y < 1.4f;
}

const char *TwoWayJet::UnitName() const {
  return "Two Way Jet";
}

const char *TwoWayJet::Author() const {
  return "JoJobananana";
}
}  // namespace battle_game::unit
