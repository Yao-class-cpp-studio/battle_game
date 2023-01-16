#include "collision_tank.h"

#include <map>

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

CollisionTank::CollisionTank(GameCore *game_core,
                             uint32_t id,
                             uint32_t player_id)
    : Tank(game_core, id, player_id) {
}

void CollisionTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void CollisionTank::InitAlreadyHit() {
  auto player = game_core_->GetPlayer(player_id_);
  auto &input_data = player->GetInputData();
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first != player_id_) {
      alreadyhit_[unit.first] = false;
    }
  }
}

void CollisionTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  AlreadyHit();
  TurretRotate();
  Fire();
}

void CollisionTank::AlreadyHit() {
  auto player = game_core_->GetPlayer(player_id_);
  auto &input_data = player->GetInputData();
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first != player_id_) {
      alreadyhit_[unit.first] = ((alreadyhit_[unit.first] == true) &&
                                 (unit.second->IsHit(position_) == true))
                                    ? true
                                    : false;
    }
  }
}

void CollisionTank::Fire() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto &units = game_core_->GetUnits();
    for (auto &unit : units) {
      if (unit.first == player_id_) {
        continue;
      }
      if ((unit.first != id_) && (unit.second->IsHit(position_)) &&
          (alreadyhit_[unit.first] == false)) {
        game_core_->PushEventDealDamage(unit.first, id_, 20.0f);
        game_core_->PushEventDealDamage(id_, unit.first, 5.0f);
        alreadyhit_[unit.first] = true;
      }
    }
  }
}

bool CollisionTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return glm::length(position) < 1.0f;
}

const char *CollisionTank::UnitName() const {
  return "Collision Tank";
}

const char *CollisionTank::Author() const {
  return "Ljy and Zkx";
}
}  // namespace battle_game::unit
