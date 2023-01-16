#include "snowball_sample_sledge.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/tiny_tank.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t sledge_body_model_index = 0xffffffffu;
uint32_t sledge_turret_model_index = 0xffffffffu;
}  // namespace

Snowball_Sample_Sledge::Snowball_Sample_Sledge(GameCore *game_core,
                                               uint32_t id,
                                               uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Antigravity mode";
  temp.description = "-50% weight, less damage in all directions";
  temp.time_remain = 0;
  temp.time_total = 360;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(Snowball_Sample_Sledge::AntigravityClick);
  skills_.push_back(temp);
  id_ = id;
};

void Snowball_Sample_Sledge::Render() {
  Tank::Render();
}

void Snowball_Sample_Sledge::AntigravityClick() {
  hidden_count_down_ = 3 * kTickPerSecond;
  antigravity_count_down_ = 6 * kTickPerSecond;
}

void Snowball_Sample_Sledge::Antigravity() {
  skills_[0].time_remain = antigravity_count_down_;
  if (antigravity_count_down_) {
    antigravity_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        AntigravityClick();
      }
    }
  }
}

void Snowball_Sample_Sledge::Antigravity_() {
  if (hidden_count_down_ == 3 * kTickPerSecond) {
    auto &units = game_core_->GetUnits();
    for (auto &unit : units) {
      if (unit.first == id_) {
        continue;
      }
      unit.second->SetWeight(0.5f * unit.second->GetWeight());
    }
    speed_ *= 2;
  }
  if (hidden_count_down_ > 0 && hidden_count_down_ < 3 * kTickPerSecond + 1) {
    Antigravity_fire();
    hidden_count_down_--;
  }
  if (hidden_count_down_ == 0) {
    auto &units = game_core_->GetUnits();
    for (auto &unit : units) {
      if (unit.first == id_) {
        continue;
      }
      unit.second->SetWeight(2.0f * unit.second->GetWeight());
    }
    speed_ /= 2;
    hidden_count_down_ = 181;
  }
}

void Snowball_Sample_Sledge::Antigravity_fire() {
  if (shoot_count_down_) {
    shoot_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      auto velocity = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_);
      for (int i = -19; i < 19; ++i) {
        auto offset = glm::radians(10.0f * i);
        velocity = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_ + offset);
        GenerateBullet<bullet::SnowBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
            turret_rotation_ + offset, 0, velocity);
      }
    }
    shoot_count_down_ = 30;
  }
}

void Snowball_Sample_Sledge::Update() {
  Tank::TankMove(speed_, glm::radians(180.0f));
  Tank::TurretRotate();
  Antigravity();
  Antigravity_();
  Fire();
}

void Snowball_Sample_Sledge::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::SnowBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

const char *Snowball_Sample_Sledge::UnitName() const {
  return "sledge_lyc";
}

const char *Snowball_Sample_Sledge::Author() const {
  return "cylic14790382";
}
}  // namespace battle_game::unit