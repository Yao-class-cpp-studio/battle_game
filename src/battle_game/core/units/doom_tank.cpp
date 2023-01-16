#include "doom_tank.h"
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include<time.h>
#include<stdlib.h>
#include<iostream>

namespace battle_game::unit {

DoomTank::DoomTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill skill;
  skill.name = "DOOMSDAY";
  skill.description = "Clear up!";
  skill.src = "../../textures/mushroom_cloud.png";
  skill.time_remain = 0;
  skill.time_total = 300;
  skill.type = E;
  skill.function = SKILL_ADD_FUNCTION(DoomTank::DestroyAll);
  skills_.push_back(skill);
  skill.name = "Leech";
  skill.description = "Health Cure";
  skill.time_remain = 0;
  skill.time_total = 360;
  skill.type = P;
  skill.function = SKILL_ADD_FUNCTION(DoomTank::ToLeech);
  skills_.push_back(skill);
  skill.name = "Shield";
  skill.description = "Shield when badly stroke";
  skill.time_remain = 0;
  skill.time_total = 300;
  skill.type = P;
  skill.function = SKILL_ADD_FUNCTION(DoomTank::ToShield);
  skills_.push_back(skill);
}

void DoomTank::Render() {
  Tank::Render();
  if (IsShield) {
    SetTransformation(position_, 0.0f, glm::vec2{1.3f});
    SetColor(game_core_->GetPlayerColor(player_id_));
    SetTexture("../../textures/shield.png");
    DrawModel(0);
  } 
}

void DoomTank::Update() {
  TankMove(4.0f, glm::radians(240.0f));
  TurretRotate();
  RandomDo();
  Doomsday();
  Leech();
  Shield();
  ToLeech();
}

bool DoomTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

// dice to determine possibility
int x() {
  int value = 0;
  srand(time(NULL));
  value = rand() % 2;
  return value + 1;
}

// 50% normal cannon ball£¬50% with lottery damage scale
void DoomTank::RandomDo() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        if (x() == 1) {
          auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
          fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        }
        if (x() == 2) {
          auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);

          for (int i = 0; i <= x(); i++) {
            GenerateBullet<bullet::CannonBall>(
                position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
                turret_rotation_, GetDamageScale(), velocity);
          }

          fire_count_down_ = kTickPerSecond;
        }
      }
    }
  }
}

//update status
void DoomTank::DoomTankUpdate() {
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    game_core_->PushEventRemoveUnit(player_id_);
  }
}

//all alive units will get damaged to different extent
void DoomTank::DestroyAll() {
  doomsday_count_down_ = 5 * kTickPerSecond;
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      game_core_->PushEventDealDamage(unit.first, id_, 30.0f);
    } else {
      game_core_->PushEventDealDamage(unit.first, id_, 60.0f);
    }
  }
  auto theta = game_core_->RandomFloat() * glm::pi<float>() * 2.0f;
  game_core_->AddObstacle<battle_game::obstacle::Crater>(
      this->GetPosition() + glm::vec2{2 * cos(theta), 2 * sin(theta)});
  DoomTankUpdate();
}

void DoomTank::Doomsday() {
  skills_[0].time_remain = doomsday_count_down_;
  if (doomsday_count_down_) {
    doomsday_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        DestroyAll();
      }
    }
  }
}

void DoomTank::Leech_() {
  skills_[1].time_remain = leech_count_down_;
  end_ = GetHealth();
  if (leech_count_down_) {
    leech_count_down_--;
  } else {
    if (begin_ - end_ >= 0.1) {
      leech_time_ = 4 * kTickPerSecond;
      leech_count_down_ = 6 * kTickPerSecond;
      tmp_begin_ = begin_;
      tmp_end_ = end_;
    }
  }
  if (leech_time_) {
    IsLeeched = true;
  } else {
    IsLeeched = false;
  }
}

void DoomTank::ToLeech() {
  if (IsLeeched) {
    SetHealth(GetHealth() + 0.003 * (tmp_begin_ - tmp_end_));
    leech_time_--;
  }
}
void DoomTank::Shield() {
  skills_[2].time_remain = shield_count_down_;
  if (shield_count_down_) {
    shield_count_down_--;
  } else {
    if (begin_ - end_ >= 0.05) {
      IsShield = true;
      InitialHealth = GetHealth();
      shield_count_down_ = 5 * kTickPerSecond;
      shield_time_ = 3 * kTickPerSecond;
      ToShield();
    }
  }

  if (IsShield) {
    if (!shield_time_) {
      IsShield = false;
      double temp;
      if (InitialHealth +
              IsLeeched * (240-leech_time_)*0.003 * (tmp_begin_ - tmp_end_) >
          GetHealth()) {
        temp = GetHealth();
      } else {
        temp = InitialHealth +
               IsLeeched * (240 - leech_time_) * 0.003 * (tmp_begin_ - tmp_end_);
      }
      SetHealth(temp);
    } else {
      shield_time_--;
    }
  }
  begin_ = end_;
}


void DoomTank::ToShield() {
  SetHealth(GetHealth() + 0.8 * (begin_ - end_));
}
  


const char *DoomTank::UnitName() const {
  return "Doom Tank";
}

const char *DoomTank::Author() const {
  return "PruBlu";
}

}  // namespace battle_game::unit
