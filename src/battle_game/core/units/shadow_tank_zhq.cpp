#include "battle_game/core/game_core.h"

namespace battle_game::unit {

ShadowTank::ShadowTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  skills_.push_back(temp);
  temp.name = "CallShadow";
  temp.description = "Generate Shadow";
  temp.time_remain = 0;
  temp.time_total = 1800;
  temp.type = Q;
  temp.function = SKILL_ADD_FUNCTION(ShadowTank::CallShadowClick);
  skills_.push_back(temp);
}

void ShadowTank::Update() {
  TankMove(5.0f, glm::radians(180.0f));
  TurretRotate();
  CallShadow();
  Fire();
}

void ShadowTank::CallShadowClick() {
  auto theta = game_core_->RandomFloat() * glm::pi<float>() * 2.0f;
  game_core_->AddUnit<battle_game::unit::Shadow>(1);
  shadow_count_down_ = 1800;
}

void ShadowTank::CallShadow() {
  skills_[1].time_remain = shadow_count_down_;
  if (shadow_count_down_) {
    shadow_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        CallShadowClick();
      }
    }
  }
}

const char *ShadowTank::UnitName() const {
  return "Shadow Tank";
}

const char *ShadowTank::Author() const {
  return "zhqwq";
}
}  // namespace battle_game::unit
