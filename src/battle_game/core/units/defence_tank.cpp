#include "defence_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

DefenceTank::DefenceTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank::Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Shield";
  temp.description = u8"按Q发动护盾";
  temp.time_remain = 0;
  temp.time_total = 60;
  temp.type = Q;
  temp.function = SKILL_ADD_FUNCTION(DefenceTank::ShieldClick);
  skills_.push_back(temp);
  temp.name = "Put Turret";
  temp.description = u8"按E放置我方炮塔";
  temp.time_remain = 0;
  temp.time_total = 180;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(DefenceTank::TurretClick);
  skills_.push_back(temp);
}

void DefenceTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
  SetShield();
  SetTurret();
  TurretsHealthDecrease();
}

void DefenceTank::TurretsHealthDecrease() {
  for (auto id : my_turret_ids_) {
    auto *turret = game_core_->GetUnit(id);
    if (!turret)
      continue;
    float health = turret->GetHealth();
    if (health <= 2e-3) {
      game_core_->PushEventRemoveUnit(id);
    } else
      turret->SetHealth(health - 2e-3);
  }
}

void DefenceTank::ShieldClick() {
  game_core_->AddUnit<unit::Shield>(player_id_, id_);
  shield_count_down_ = 10 * kTickPerSecond;
}

void DefenceTank::SetShield() {
  skills_[0].time_remain = shield_count_down_;
  if (shield_count_down_) {
    shield_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        ShieldClick();
      }
    }
  }
}

void DefenceTank::TurretClick() {
  uint32_t turret_id =
      game_core_->AddUnit<unit::NeutralTurret>(player_id_, false);
  my_turret_ids_.push_back(turret_id);
  game_core_->GetUnit(turret_id)->SetPosition(position_ +
                                              glm::vec2{0.0f, -1.5f});
  turret_count_down_ = 10 * kTickPerSecond;
}

void DefenceTank::SetTurret() {
  skills_[1].time_remain = turret_count_down_;
  if (turret_count_down_) {
    turret_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        TurretClick();
      }
    }
  }
}

const char *DefenceTank::UnitName() const {
  return "Defence Tank";
}

const char *DefenceTank::Author() const {
  return "CoffeePot";
}

}  // namespace battle_game::unit
