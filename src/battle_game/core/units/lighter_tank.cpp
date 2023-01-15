#include "lighter_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

LighterTank::LighterTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Block";
  temp.description = "Add a new WoodenBlock";
  temp.time_remain = 0;
  temp.time_total = 900;
  temp.type = Q;
  temp.function = SKILL_ADD_FUNCTION(LighterTank::BlockClick);
  skills_.push_back(temp);
}

void LighterTank::Render() {
  Tank::Render();
}

void LighterTank::Update() {
  TankMove(8.0f, glm::radians(360.0f));
  TurretRotate();
  Block();
  Fire();
}

void LighterTank::Block() {
  skills_[0].time_remain = block_count_down_;
  if (block_count_down_) {
    block_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        BlockClick();
      }
    }
  }
}

void LighterTank::BlockClick() {
  auto theta = game_core_->RandomFloat() * glm::pi<float>() * 2.0f;
  game_core_->AddObstacle<battle_game::obstacle::WoodenBlock>(
      this->GetPosition() + glm::vec2{2 * cos(theta), 2 * sin(theta)});
  block_count_down_ = 15 * kTickPerSecond;
}

void LighterTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::LightCannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool LighterTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *LighterTank::UnitName() const {
  return "Lighter Tank";
}

const char *LighterTank::Author() const {
  return "RyanZhang";
}
}  // namespace battle_game::unit