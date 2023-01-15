#include "bullet_storage_tank.h"
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

BulletStorageTank::BulletStorageTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void BulletStorageTank::Render() {
  Tank::Render();
}

void BulletStorageTank::Update() {
  TankMove(7.0f, glm::radians(180.0f));
  TurretRotate();
  BulletStorage();
}

bool BulletStorageTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *BulletStorageTank::UnitName() const {
  return "Bullet Storage Tank";
}

const char *BulletStorageTank::Author() const {
  return "ZiXi Cai";
}

void BulletStorageTank::BulletStorage() {
  if (bullet_storage_ <= 5 * kTickPerSecond)
    bullet_storage_++;
  if (fire_count_down_ != 0 && fire_count_down_ != 1 && fire_count_down_ != 12)
    fire_count_down_--;
  else if (bullet_storage_ >= kTickPerSecond) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] || fire_count_down_ == 1) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = 11;
        bullet_storage_ -= kTickPerSecond;
        if(bullet_storage_ < kTickPerSecond)
          fire_count_down_ = kTickPerSecond;
      }
    }
  }
}

}  // namespace battle_game::unit
