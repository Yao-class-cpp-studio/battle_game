#include "battle_game/core/bullets/laser.h"

#include <math.h>

#include <iostream>

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
#define pi acos(-1)
#define speed 7
namespace battle_game::bullet {
Laser::Laser(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity,
             int LightIndex)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      end_position_(position),
      color_(game_core_->GetPlayerColor(player_id)),
      LightIndex_(LightIndex),
      ComboTime(0),
      Original_velocity_(velocity) {
  switch (LightIndex_) {
    case 0:
      color_ = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
      break;
    case 1:
      color_ = glm::vec4{1.0f, 0.0f, 0.0f, 1.0f};
      break;
    case 2:
      color_ = glm::vec4{1.0f, 0.5f, 0.0f, 1.0f};
      break;
    case 3:
      color_ = glm::vec4{1.0f, 1.0f, 0.0f, 1.0f};
      break;
    case 4:
      color_ = glm::vec4{0.0f, 1.0f, 0.0f, 1.0f};
      break;
    case 5:
      color_ = glm::vec4{0.0f, 0.0f, 0.7f, 1.0f};
      break;
    case 6:
      color_ = glm::vec4{1.0f, 0.0f, 1.0f, 1.0f};
      break;
  }
}
void Laser::update_color(float &a) {
  a += 0.4 * (game_core_->RandomFloat() - 0.5);
  a = a > 0 ? a : 0;
  a = a < 1 ? a : 1;
}
void Laser::Render() {
  if (ComboTime * speed > 1500) {
    update_color(color_.x);
    update_color(color_.y);
    update_color(color_.z);
  }
  int Distance = 0;
  auto tmp_position_ = position_;
  auto Color = color_;
  Color.w = Transparent_();
  SetTexture("../../textures/particle3.png");
  while (!game_core_->IsBlockedByObstacles(
      tmp_position_ +
      glm::vec2{Original_velocity_.x * 0.06, Original_velocity_.y * 0.06})) {
    Distance++;
    float white = White_(Distance);
    SetColor(
        glm::clamp(glm::vec4{color_.x * (1 - white) + white,
                             color_.y * (1 - white) + white,
                             color_.z * (1 - white) + white, Transparent_()},
                   glm::vec4{0}, glm::vec4{1}));
    tmp_position_ += velocity_ * (kSecondPerTick / 20);
    SetTransformation(tmp_position_ + glm::vec2{Original_velocity_.x * 0.06,
                                                Original_velocity_.y * 0.06},
                      rotation_, glm::vec2{Width_()});
    DrawModel(0);
  }
  // std::cout << Distance << std::endl;
}

void Laser::Update() {
  ComboTime++;
  auto player = game_core_->GetPlayer(player_id_);
  auto &units = game_core_->GetUnits();
  if (!player)
    return;
  auto &input_data = player->GetInputData();
  auto diff = input_data.mouse_cursor_position - position_;
  if (glm::length(diff) > 1e-4) {
    auto rotation_ = std::atan2(diff.y, diff.x);
    float Total_velocity_ =
        std::sqrt(Original_velocity_.x * Original_velocity_.x +
                  Original_velocity_.y * Original_velocity_.y);
    Original_velocity_.x = Total_velocity_ * std::cos(rotation_);
    Original_velocity_.y = Total_velocity_ * std::sin(rotation_);
    rotation_ += Angle_();
    float total_velocity_ =
        std::sqrt(velocity_.x * velocity_.x + velocity_.y * velocity_.y);
    velocity_.x = total_velocity_ * std::cos(rotation_);
    velocity_.y = total_velocity_ * std::sin(rotation_);
  }
  if (!input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
    game_core_->PushEventRemoveBullet(id_);
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      position_ = unit.second->GetPosition();
      break;
    }
  }
  end_position_ = position_;
  bool ended = false;
  while (!ended) {
    end_position_ += velocity_ * kSecondPerTick;
    if (game_core_->IsBlockedByObstacles(end_position_)) {
      ended = true;
    }

    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        // position_=unit.second->GetPosition();
        continue;
      }
      if (unit.second->IsHit(end_position_)) {
        game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 0.08f);
      }
    }
  }
  if (LightIndex_ == 1)
    std::cout << ComboTime << ' ' << Width_() << std::endl;
  /*if (--count_down_ == 0)
      game_core_->PushEventRemoveBullet(id_);*/
}
// TODO

float Laser::angle() {
  return 0.01 * (pow(ComboTime * speed, 1.15)) + 3 * LightIndex_ / pi;
}

float Laser::Angle_() {
  return cos(angle()) * std::max(exp(-0.002 * ComboTime * speed) - 0.07, 0.0);
}

float Laser::Transparent_() {
  if (ComboTime * speed > 1500) {
    return 0.7;
  }
  return std::min(
      1.0, (0.01 * sin(angle()) + 0.012) * (1 + ComboTime * speed / 1000));
}

float Laser::Width_() {
  return std::min(0.3, 0.007 * pow(ComboTime * speed, 0.5) + 0.07);
}

float Laser::White_(int Distance) {
  if (ComboTime * speed > 1500) {
    return 0.7;
  }
  return std::min(0.5, 0.2 + ComboTime * speed / (700 * log(Distance)));
}
Laser::~Laser() {
  game_core_->PushEventRemoveBullet(id_);
}
}  // namespace battle_game::bullet
