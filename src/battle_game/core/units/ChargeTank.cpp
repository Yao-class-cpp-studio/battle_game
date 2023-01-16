#include "ChargeTank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include <time.h>

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

ChargeTank::ChargeTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void ChargeTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}


void ChargeTank::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
      if(count_ == 1){
        end_ = clock();
        // Soar();
        float t = (end_ - start_) / CLOCKS_PER_SEC;
        if ( t > 0.1f && t < 0.32f){
          if ((clock() - count_down_) / CLOCKS_PER_SEC >= 1.0f){
            count_ = 0;
            offset.y += 100.0f;
            glm::vec2 direc = glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                              glm::vec3{0.0f, 0.0f, 1.0f}) *
                                  glm::vec4{offset, 0.0f, 0.0f}};
            GenerateBullet<bullet::ChargeFire>(
            position_ + Rotate({0.0f, 1.2f}, rotation_),
            rotation_ + glm::radians(180.0f), 0.6 * GetDamageScale(), 20.0f * glm::normalize(direc));
            count_down_ = clock();
          }
        } else {
          count_ = 1;
          start_ = clock ();
          offset.y += 1.0f;
        }
        //
      } else if(count_ == 0){
        count_ = 1;
        start_ = clock();
        offset.y += 1.0f;
      }
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

const char *ChargeTank::UnitName() const {
  return "ChargeTank";
}

const char *ChargeTank::Author() const {
  return "TangLi";
}
}  // namespace battle_game::unit
