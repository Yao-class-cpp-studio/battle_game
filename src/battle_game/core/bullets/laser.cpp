#include "battle_game/core/bullets/laser.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
Laser::Laser(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale,
                       glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity),
      end_position_(position),
      count_down_(150),
      color_(game_core_->GetPlayerColor(player_id)) {
}

void Laser::UpdateColor(float &a) {
  a += 0.4 * (game_core_->RandomFloat() - 0.5);
  a = a > 0 ? a : 0;
  a = a < 1 ? a : 1;
}

void Laser::Render() {
  auto tmp_position_ = position_;
    UpdateColor(color_.r);
    UpdateColor(color_.b);
    UpdateColor(color_.g);
    SetColor(color_);
  SetTexture("../../textures/particle3.png");
  while (!game_core_->IsBlockedByObstacles(tmp_position_) 
      || game_core_->IsStuckBySwamp(tmp_position_)) {
    tmp_position_ += velocity_ * (kSecondPerTick / 20);
    SetTransformation(tmp_position_, rotation_, glm::vec2{0.05f});
    DrawModel(0);
  }
}

void Laser::Update() {
  auto player = game_core_->GetPlayer(player_id_);
  if (!player)
    return;
  auto &input_data = player->GetInputData();
  if (!input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
    game_core_->PushEventRemoveBullet(id_);
  auto &units = game_core_->GetUnits();
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) > 1e-4) {
      auto rotation_ = std::atan2(diff.y, diff.x);
      float total_velocity_ =
          std::sqrt(velocity_.x * velocity_.x + velocity_.y * velocity_.y);
      velocity_.x = total_velocity_ * std::cos(rotation_);
      velocity_.y = total_velocity_ * std::sin(rotation_);
    }
  }
  
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

    if (game_core_->IsBlockedByObstacles(end_position_) &&
        !game_core_->IsStuckBySwamp(end_position_)) {
      ended = true;
    }
    
    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        continue;
      } 
      if (unit.second->IsHit(end_position_)) {
        game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 0.08f);
      }
    }
  }
  
  if (--count_down_ == 0)
    game_core_->PushEventRemoveBullet(id_);
}

Laser::~Laser() {}
}  // namespace battle_game::bullet*/
