#include "battle_game/core/units/phantom.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

Phantom::Phantom(GameCore *game_core, uint32_t id, uint32_t player_id)
    : InfernoTank(game_core, id, player_id) {
}

int Phantom::GetRemainDodgeTime() const {
  return remain_dodge_time_;
}

void Phantom::SetRemainDodgeTime(int new_remain_dodge_time) {
  remain_dodge_time_ = new_remain_dodge_time;
}

void Phantom::Render() {
  battle_game::SetTransformation(position_, rotation_);
  std::string phantom_pic_path =
      std::string("../../textures/phantoms/phantom") +
      char('0' + remain_dodge_time_) + std::string(".png");
  battle_game::SetTexture(phantom_pic_path.c_str());
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(
      0);  // cannot tauch \ tank_body_model_index /, so do turrent
  battle_game::SetTransformation(position_, turret_rotation_,
                                 glm::vec2{2.0f, 2.0f});
  battle_game::SetTexture("../../textures/phantoms/phantom_turrent.png");
  battle_game::DrawModel();
}

const char *Phantom::UnitName() const {
  return "Phantom";
}

const char *Phantom::Author() const {
  return "hwj";
}

}  // namespace battle_game::unit
