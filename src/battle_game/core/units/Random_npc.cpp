#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/Random_tank.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

RandomNPC::RandomNPC(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void RandomNPC::Render() {
  Tank::Render();
}

void RandomNPC::RandomMove() {
  float move_speed = 3.0f;
  float rotate_angular_speed = 180.0f;
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (current_steps_ == 0) {
      current_steps_ = 30;
      current_direction_ = game_core_->RandomInt(1, 2);
    }
    current_steps_--;
    if (current_direction_ == 1) {
      offset.y += 1.0f;
    } else {
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
    if (game_core_->RandomInt(1, 50) == 1) {
      rotation_offset += 1.0f;
    }
    if (game_core_->RandomInt(1, 50) == 1) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void RandomNPC::RandomTurretRotate() {
  if (current_steps_ % 10 != 0) {
    return;
  }
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    turret_rotation_ = 180.0f * game_core_->RandomFloat() - glm::radians(90.0f);
  }
}
void RandomNPC::Update() {
  RandomMove();
  RandomTurretRotate();
  Fire();
}

void RandomNPC::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (game_core_->RandomInt(1, 3) == 1) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        int tms = game_core_->RandomInt(1, 5);
        for (int i = 0; i < tms; i++) {
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
          fire_count_down_ = 2 * kTickPerSecond;  // Fire interval 2 second.
        }
        if (tms == 4 && game_core_->npc_difficulty >= 5) {
          GenerateBullet<bullet::WaterDrop>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
        }
      }
    }
  }
}

bool RandomNPC::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

float RandomNPC::GetDamageScale() const {
  return 0.5f + 0.05f * game_core_->npc_difficulty;
}

float RandomNPC::BasicMaxHealth() const {
  return 50.0f + 5.0f * game_core_->npc_difficulty;
}
const char *RandomNPC::UnitName() const {
  return "Random NPC";
}  // however,this should not be selectable

const char *RandomNPC::Author() const {
  return "JohnVictor36";
}
}  // namespace battle_game::unit