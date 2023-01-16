#include "TankA.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

TankA::TankA(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill qwq;
  qwq.name = "Angel";
  qwq.time_remain = 0;
  qwq.time_total = 5 * kTickPerSecond;
  qwq.type = E;
  qwq.function = SKILL_ADD_FUNCTION(TankA::AngelClick);
  skills_.push_back(qwq);
}

void TankA::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Angel();
  Fire();
}

void TankA::AngelClick() {
  angel_count_down_ = 5 * kTickPerSecond;

  // create angel
  auto unit = game_core_->GetUnit(
      game_core_->AddUnit<battle_game::unit::TankAngel>(player_id_));
  unit->SetPosition(position_ +
                    Rotate(glm::vec2{0.0f, 1.6f}, turret_rotation_));
  unit->SetRotation(rotation_);
  // game_core_->PushEventMoveUnit(unit, position_ +
  // Rotate(glm::vec2{0.0f, 2.0f}, turret_rotation_));
  // game_core_->PushEventRotateUnit(unit, rotation_);
}

void TankA::Angel() {
  skills_[0].time_remain = angel_count_down_;
  if (angel_count_down_)
    --angel_count_down_;
  else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E])
        AngelClick();
    }
  }
}

const char *TankA::UnitName() const {
  return "TankA";
}

const char *TankA::Author() const {
  return "Starusc";
}
}  // namespace battle_game::unit
