#include "all_round_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

AllRoundTank::AllRoundTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill speed_mode;
  speed_mode.name = "Speed Mode";
  speed_mode.description =
      "In this mode, the speed of your tank increases by 40%.";
  speed_mode.time_remain = 0;
  speed_mode.time_total = 0;
  speed_mode.type = SkillType::E;
  speed_mode.function = SKILL_ADD_FUNCTION(AllRoundTank::SpeedMode);
  skills_.push_back(speed_mode);

  Skill damage_mode;
  damage_mode.name = "Damage Mode";
  damage_mode.description =
      "In this mode, the damage your tank done increases by 30%.";
  damage_mode.time_remain = 0;
  damage_mode.time_total = 0;
  damage_mode.type = SkillType::Q;
  damage_mode.function = SKILL_ADD_FUNCTION(AllRoundTank::DamageMode);
  skills_.push_back(damage_mode);

  Skill recovery_mode;
  recovery_mode.name = "Recovery Mode";
  recovery_mode.description =
      "In this mode, your tank recovers 3% of health per second.";
  recovery_mode.time_remain = 0;
  recovery_mode.time_total = 0;
  recovery_mode.type = SkillType::R;
  recovery_mode.function = SKILL_ADD_FUNCTION(AllRoundTank::RecoveryMode);
  skills_.push_back(recovery_mode);
}

float AllRoundTank::GetDamageScale() const {
  return damage_scale_;
}

float AllRoundTank::GetSpeedScale() const {
  return speed_scale_;
}

void AllRoundTank::SpeedMode() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    current_mode_ = 0;
    speed_scale_ = 1.4f;
    damage_scale_ = 1.0f;
  }
}

void AllRoundTank::DamageMode() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    current_mode_ = 1;
    speed_scale_ = 1.0f;
    damage_scale_ = 1.3f;
  }
}

void AllRoundTank::RecoveryMode() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    current_mode_ = 2;
    speed_scale_ = 1.0f;
    damage_scale_ = 1.0f;
  }
}

void AllRoundTank::SwitchMode() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.key_down[GLFW_KEY_R]) {
      RecoveryMode();
    } else if (input_data.key_down[GLFW_KEY_Q]) {
      DamageMode();
    } else if (input_data.key_down[GLFW_KEY_E]) {
      SpeedMode();
    }
  }
  if (current_mode_ == 2) {
    SetHealth(health_ + 0.03 / kTickPerSecond);
  }
}

void AllRoundTank::Update() {
  SwitchMode();
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

const char *AllRoundTank::UnitName() const {
  return "All-Round Tank";
}

const char *AllRoundTank::Author() const {
  return "InvUsr";
}
}  // namespace battle_game::unit