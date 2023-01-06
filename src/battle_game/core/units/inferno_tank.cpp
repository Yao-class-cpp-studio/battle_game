#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/double_scatter_tank.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

InfernoTank::InfernoTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Hide";
  temp.description = "Fast bullet";
  temp.time_remain = 0;
  temp.time_total = 360;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(InfernoTank::HiddenClick);
  skills_.push_back(temp);
  temp.name = "Block";
  temp.description = "Generate Block";
  temp.time_remain = 0;
  temp.time_total = 900;
  temp.type = Q;
  temp.function = SKILL_ADD_FUNCTION(InfernoTank::BlockClick);
  skills_.push_back(temp);
  temp.name = "Rocket";
  temp.description = "Little Bullet";
  temp.time_remain = 0;
  temp.time_total = 0;
  temp.bullet_type = 1;
  temp.bullet_total_number = 1;
  temp.type = B;
  skills_.push_back(temp);
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

void InfernoTank::HiddenClick() {
  isHidden = 3 * kTickPerSecond;
  hidden_count_down_ = 6 * kTickPerSecond;
}
void InfernoTank::BlockClick() {
  auto theta = game_core_->RandomFloat() * glm::pi<float>() * 2.0f;
  game_core_->AddObstacle<battle_game::obstacle::Block>(
      this->GetPosition() + glm::vec2{2 * cos(theta), 2 * sin(theta)});
  block_count_down_ = 15 * kTickPerSecond;
  isHidden += 3 * kTickPerSecond;
}

void InfernoTank::Hidden() {
  skills_[0].time_remain = hidden_count_down_;
  if (hidden_count_down_) {
    hidden_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        HiddenClick();
      }
    }
  }
}

void InfernoTank::Block() {
  skills_[1].time_remain = block_count_down_;
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
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] &&
          game_core_->GetUnits().size() > 1) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Rocket>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
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
