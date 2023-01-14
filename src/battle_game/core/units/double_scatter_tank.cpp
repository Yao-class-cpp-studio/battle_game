#include "battle_game/core/units/double_scatter_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

DoubleScatterTank::DoubleScatterTank(GameCore *game_core,
                                     uint32_t id,
                                     uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void DoubleScatterTank::Render() {
  Tank::Render();
}

void DoubleScatterTank::Update() {
  if (!is_stuck_by_swamp_) {
    TankMove(3.0f, glm::radians(180.0f));
  } else {
    TankMove(1.5f, glm::radians(180.0f));
  }
  TurretRotate();
  Fire();
}

void DoubleScatterTank::Scatter() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    for (int i = -3; i <= 3; ++i) {
      auto offset = glm::radians(3.0f * i);
      velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
      GenerateBullet<bullet::CannonBall>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
          turret_rotation_ + offset, GetDamageScale(), velocity);
    }
  }
}

void DoubleScatterTank::Fire() {
  // check whether to perform second scatter
  if (second_scatter_count_down_) {
    second_scatter_count_down_--;
  } else {
    if (is_second_scatter_) {
      auto player = game_core_->GetPlayer(player_id_);
      if (player) {
        Scatter();
        is_second_scatter_ = false;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        Scatter();
        // wait for 0.5 second
        second_scatter_count_down_ = kTickPerSecond / 4;
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        is_second_scatter_ = true;
      }
    }
  }
}

bool DoubleScatterTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *DoubleScatterTank::UnitName() const {
  return "Double Scatter Tank";
}

const char *DoubleScatterTank::Author() const {
  return "LinTX";
}
}  // namespace battle_game::unit
