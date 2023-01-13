#include "smoke_bomb_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

SmokeBombTank::SmokeBombTank(GameCore *game_core,
                             uint32_t id,
                             uint32_t player_id)
    : Tank(game_core, id, player_id),
      aimed_(false),
      max_distance_(10.0f),
      radius_(4.0f) {
}

void SmokeBombTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

bool SmokeBombTank::AimAt(const glm::vec2 &position) {
  if (game_core_->IsOutOfRange(position)) {
    return false;
  }
  auto diff = position - position_;
  auto length = glm::length(diff);
  if (length <= 1.2f) {
    return false;
  } else if (length <= max_distance_) {
    target_ = position;
  } else {
    target_ = position_ + diff / glm::length(diff) * max_distance_;
  }
  return true;
}

void SmokeBombTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      aimed_ = AimAt(input_data.mouse_cursor_position);
      if (aimed_ && input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        aimed_ = false;
        GenerateBullet<bullet::SmokeBomb>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), target_, radius_, 1.0f, 1.0f);
        fire_count_down_ = kTickPerSecond * 5;  // Fire interval 5 seconds.
      }
    }
  }
}

void SmokeBombTank::RenderHelper() {
  if (aimed_) {
    const int precision = 60;
    const float inv_precision = 1.0f / float(precision);
    for (int i = 0; i < precision; i++) {
      auto theta = (float(i) + 0.5f) * inv_precision;
      theta *= glm::pi<float>() * 2.0f;
      auto sin_theta = std::sin(theta);
      auto cos_theta = std::cos(theta);
      auto position =
          target_ + glm::vec2{cos_theta * radius_, sin_theta * radius_};
      if (!game_core_->IsOutOfRange(position)) {
        battle_game::SetTransformation(position, 0.0f, glm::vec2{0.05f});
        battle_game::SetColor(glm::vec4{1.0f, 0.5f, 0.5f, 1.0f});
        battle_game::SetTexture("../../textures/particle3.png");
        battle_game::DrawModel(0);
      }
    }
  }
}

const char *SmokeBombTank::UnitName() const {
  return "Smoke Bomb Tank";
}

const char *SmokeBombTank::Author() const {
  return "ljc1301";
}
}  // namespace battle_game::unit
