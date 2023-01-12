#include "battle_game/core/units/jojo_3.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

KunKun::KunKun(GameCore *game_core, uint32_t id, uint32_t player_id)
    : JoJo(game_core, id, player_id) {
}

void KunKun::Render() {
  JoJo::Render();
}

void KunKun::Update() {
  JoJoMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void KunKun::ShootBasketball() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    for (int i = 0; i <= 14; ++i) {
      auto offset = glm::radians(24.0f * i);
      velocity = Rotate(glm::vec2{0.0f, 1.0f}, turret_rotation_ + offset);
      GenerateBullet<bullet::BasketBall>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
          turret_rotation_ + offset, GetDamageScale(), velocity);
    }
  }
}

void KunKun::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Chicken>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }

  if (basketball_count_down_) {
    basketball_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_RIGHT]) {
        ShootBasketball();
        basketball_count_down_ =
            10 * kTickPerSecond;  // Fire interval 10 second.
      }
    }
  }
}

bool KunKun::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -1.0f && position.x < 1.0f && position.y > -1.4f &&
         position.y < 1.4f;
}

const char *KunKun::UnitName() const {
  return "KunKun";
}

const char *KunKun::Author() const {
  return "JoJobananana";
}
}  // namespace battle_game::unit
