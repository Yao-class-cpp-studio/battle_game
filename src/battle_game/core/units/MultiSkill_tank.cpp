#include "MultiSkill_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
MultiTank::MultiTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
    }

void MultiTank::Render() {
  Tank::Render();
}

void MultiTank::Update() {
  TankMove(10.0f, glm::radians(180.0f));
  TurretRotate();
  MultiFire();
  Shield();
}

void MultiTank::MultiFire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        firetime_ += 1;
        if (firetime_ % 20 == 5) {
            GenerateBullet<bullet::ReboundingBall>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, GetDamageScale(), velocity);
        }
        if (firetime_ % 20 == 10) {
            GenerateBullet<bullet::Rocket>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, GetDamageScale(), velocity);
        }
        if (firetime_ % 20 == 15) {
            Shield();
        }
        if (firetime_ % 20 == 0) {
            GenerateBullet<bullet::WaterDrop>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, GetDamageScale(), velocity);
        }
        if (IsHit(position_))
          fire_count_down_ = kTickPerSecond;
        else
          fire_count_down_ = kTickPerSecond * 2;  // Fire interval 2 second.
      }
    }
  }
}

void MultiTank::Shield() {
  skills_[0].time_remain = protect_time_;
  if (protect_time_) {
    protect_time_--;
  } else if (be_protected == false) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        protect_time_ = 10 * kTickPerSecond;
        be_protected = true;
      }
    }
  }
}

bool MultiTank::IsHit(glm::vec2 position) const {
  if (be_protected == true)
    return false; 
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

  const char *MultiTank::UnitName() const {
  return "Multibullet Tank";
}

const char *MultiTank::Author() const {
  return "jq_zhang";
}
}  // namespace battle_game::unit
