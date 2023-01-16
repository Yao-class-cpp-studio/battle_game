#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "soilder_Xiaoyu.h"

namespace battle_game::unit {

Soilder::Soilder(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void Soilder::Render() {
  Tank::Render();
}

void Soilder::Update() {
  soilderMove(6.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void Soilder::SoilderMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 2.5f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 2.5f;
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
      rotation_offset += 3.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 3.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void Soilder::Fire() {
  Tank::Fire();
}

bool Soilder::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *Soilder::UnitName() const {
  return "Handsome Soilder";
}

const char *Soilder::Author() const {
  return "Xiaoyu";
}
}  // namespace battle_game::unit
