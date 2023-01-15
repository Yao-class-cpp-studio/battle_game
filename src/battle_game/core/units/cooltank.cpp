#include "cooltank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

CoolTank::CoolTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void CoolTank::Render() {
  Tank::Render();
}

void CoolTank::Update() {
  TankMove(3.8f, glm::radians(180.0f));
  TurretRotate();
  CoolFire();
}

void CoolTank::Addhitnumber(){
  hit_number_++;
}

void CoolTank::CoolFire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Coolbullet>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        if (hit_number_ == 3){
          GenerateBullet<bullet::CannonBall>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, GetDamageScale(), velocity);
        }
        hit_number_=0;
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool CoolTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *CoolTank::UnitName() const {
  return "CoolTank";
}

const char *CoolTank::Author() const {
  return "tangtang";
}
}  // namespace battle_game::unit