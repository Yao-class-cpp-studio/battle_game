#include "tank_yfl.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"
namespace battle_game::unit {

TankYfl::TankYfl(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill skill;
  skill.name = "SpeedUp";
  skill.description = "2 times speed for 3 seconds";
  skill.time_remain = 0;
  skill.time_total = 480;
  skill.type = E;
  skill.function = SKILL_ADD_FUNCTION(TankYfl::SpeedUpClick);
  skills_.push_back(skill);
}

void TankYfl::SpeedUpClick() {
  IsSpeed = 3 * kTickPerSecond;
  speedup_count_down = 8 * kTickPerSecond;
}

void TankYfl::SpeedUp() {
  skills_[0].time_remain = speedup_count_down;
  if (IsSpeed) {
    IsSpeed--;
  }
  if (speedup_count_down) {
    speedup_count_down--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        SpeedUpClick();
      }
    }
  }
}

float TankYfl::GetSpeedScale() const {
    if (IsSpeed) {
        return 3.5f;
  } else
    return 1.0f;
}

void TankYfl::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
  SpeedUp();
}

const char *TankYfl::UnitName() const {
  return "Tank Yfl";
}

const char *TankYfl::Author() const {
  return "Yfl";
}
}  // namespace battle_game::unit
