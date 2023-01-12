#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "battle_game/core/units/yzc_tank.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

Tankyzc::Tankyzc(GameCore *game_core,uint32_t id,uint32_t player_id)                        
    : Tank(game_core, id, player_id) {
}

void Tankyzc::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  BigFire();
}

/////////////////////////////////////////////////
//////////////////200 health/////////////////////
/////////////////////////////////////////////////
float Tankyzc::BasicMaxHealth() const {
  return 200.0f;
}

//////////////////////////////////////////////////
///////////////////Modify Fire////////////////////
/////////////////////////////////////////////////

void Tankyzc::BigFire() {
  if (fire_count_down_==0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {

        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        glm::vec2 position = position_ + Rotate(glm::vec2{0.0f, 1.5f}, turret_rotation_);
        float rotation = turret_rotation_;
        float damage_scale = 0.05f;

        GenerateBullet<bullet::CannonBall>(position,rotation,damage_scale, velocity);
        fire_count_down_ = 239;
      }
    }
  } 
  else if (fire_count_down_ >= 120) {
    if (fire_count_down_ % 3 == 0) {
      auto player = game_core_->GetPlayer(player_id_);
      if (player) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        glm::vec2 position =
            position_ + Rotate(glm::vec2{0.0f, 1.5f}, turret_rotation_);
        float rotation = turret_rotation_;
        float damage_scale = 0.05f;

        GenerateBullet<bullet::CannonBall>(position, rotation, damage_scale, velocity); 
      }
    }
    fire_count_down_--;
  }
  else {
    fire_count_down_--;
  }
}

const char *Tankyzc::UnitName() const {
  return "Yankyzc";
}

const char *Tankyzc::Author() const {
  return "YZC";
}

}  // namespace battle_game::unit
