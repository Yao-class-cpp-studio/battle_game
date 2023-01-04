#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/double_scatter_tank.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

InfernoTank::InfernoTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void InfernoTank::Render() {
  Tank::Render();
}

void InfernoTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  Inferno();
  Hidden();
  Block();
  Fire();
}

void InfernoTank::Hidden() {
  if (hidden_count_down_) {
    hidden_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        isHidden = 3 * kTickPerSecond;
        hidden_count_down_ = 6 * kTickPerSecond;
      }
    }
  }
}

void InfernoTank::Block() {
  if (block_count_down_) {
    block_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        auto theta = game_core_->RandomFloat() * glm::pi<float>() * 2.0f;
        game_core_->AddObstacle<battle_game::obstacle::Block>(
            this->GetPosition() + glm::vec2{2 * cos(theta), 2 * sin(theta)});
        block_count_down_ = 15 * kTickPerSecond;
        isHidden += 4 * kTickPerSecond;
      }
    }
  }
}

void InfernoTank::Inferno() {
  if (isHidden > 0) {
    if (shoot_count_down_ > 30)
      shoot_count_down_ = 30;
    isHidden--;
  }
  if (shoot_count_down_) {
    shoot_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
      for (int i = -9; i < 9; ++i) {
        auto offset = glm::radians(20.0f * i);
        velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
            turret_rotation_ + offset, GetDamageScale(), velocity);
      }
    }
    if (isHidden > 0) {
      shoot_count_down_ = 30;
    } else
      shoot_count_down_ = 4 * kTickPerSecond;
  }
}

void InfernoTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        for (int i = -3; i <= 3; i += 2) {
          auto offset = glm::radians(3.0f * i);
          velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
          GenerateBullet<bullet::Rocket>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
              turret_rotation_ + offset, GetDamageScale(), velocity);
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool InfernoTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *InfernoTank::UnitName() const {
  return "Inferno Tank";
}

const char *InfernoTank::Author() const {
  return "Xkev";
}
}  // namespace battle_game::unit
