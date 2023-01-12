#include "crit_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
CritTank::CritTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Blessings of the abyssal moon";
  temp.description = "The crtical chance and critical damage increase";
  temp.time_remain = 0;
  temp.time_total = 600;
  temp.type = E;
  skills_.push_back(temp);
}

void CritTank::Render() {
  Tank::Render();
}

void CritTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
  BlessingsOfTheAbyssalMoon();
}

void CritTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        float crit_chance = crit_chance_;
        float crit_damage = crit_damage_;
        GenerateBullet<bullet::CritBullet>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity, crit_chance,
            crit_damage);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

void CritTank::BlessingsOfTheAbyssalMoon() {
  skills_[0].time_remain = blessings_count_down_;
  if (blessings_count_down_) {
    blessings_count_down_--;
    if (blessings_duration_) {
      blessings_duration_--;
      for (int i = 0; i < 5; i++) {
        game_core_->PushEventGenerateParticle<particle::Smoke>(
            position_, rotation_, game_core_->RandomInCircle() * 6.0f, 0.5f,
            glm::vec4{game_core_->RandomFloat(), game_core_->RandomFloat(),
                      game_core_->RandomFloat(), game_core_->RandomFloat()},
            3.0f);
      }
    } else {
      crit_chance_ = 0.3f;
      crit_damage_ = 0.8f;
    }
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        crit_chance_ = 1.0f;
        crit_damage_ = 1.5f;
        blessings_count_down_ = 10 * kTickPerSecond;
        blessings_duration_ = 3 * kTickPerSecond + 30;
      }
    }
  }
}

bool CritTank::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

const char *CritTank::UnitName() const {
  return "Crit Tank";
}

const char *CritTank::Author() const {
  return "Aoarashi";
}
}  // namespace battle_game::unit
