#include "fallen_angel.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
FallenAngel::FallenAngel(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "The power from Abyss";
  temp.description =
      "reduce 20 percent of max life and lose 2.5 percent of life per "
      "second(only if life is more than 40 percent), change the bullet to the "
      "burning bullet which damage scale is according to the current life(5 "
      "percent of damage scale multiplier for every 1 percent of health lost "
      "compared to 80% maximum health)";
  temp.time_remain = 0;
  temp.time_total = 360;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(FallenAngel::ThePowerFromAbyssClick);
  skills_.push_back(temp);
  temp.name = "The salvation from Stars";
  temp.description = "restore 40 percent of max life";
  temp.time_remain = 0;
  temp.time_total = 900;
  temp.type = Q;
  temp.function = SKILL_ADD_FUNCTION(FallenAngel::TheSalvationFromStarsClick);
  skills_.push_back(temp);
  temp.name = "The bless of God";
  temp.description =
      "When the current life is lesser than 50 percent of max life, the damage "
      "of bullet will increase by 50 percent";
  temp.type = P;
  skills_.push_back(temp);
}

void FallenAngel::Render() {
  Tank::Render();
}

void FallenAngel::Update() {
  TankMove(6.0f, glm::radians(270.0f));
  TurretRotate();
  ThePowerFromAbyss();
  TheSalvationFromStars();
  TheBlessOfGod();
  Fire();
}

void FallenAngel::ThePowerFromAbyssClick() {
  is_burning_ = 4 * kTickPerSecond;
  the_power_from_abyss_count_down_ = 6 * kTickPerSecond;
  if (health_ > 0.4) {
    health_ -= 0.2;
  }
}

void FallenAngel::TheSalvationFromStarsClick() {
  the_salvation_from_stars_count_down_ = 15 * kTickPerSecond;
  health_ += 0.4;
  if (health_ > 1) {
    health_ = 1;
  }
}

void FallenAngel::TheBlessOfGod() {
  if (health_ < 0.5) {
    is_blessed_ = true;
  } else {
    is_blessed_ = false;
  }
}

void FallenAngel::ThePowerFromAbyss() {
  skills_[0].time_remain = the_power_from_abyss_count_down_;
  if (the_power_from_abyss_count_down_) {
    the_power_from_abyss_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        ThePowerFromAbyssClick();
      }
    }
  }
  if (is_burning_) {
    if (health_ > 0.2) {
      health_ -= 0.025 / 60;
    }
    is_burning_--;
  }
}

void FallenAngel::TheSalvationFromStars() {
  skills_[1].time_remain = the_salvation_from_stars_count_down_;
  if (the_salvation_from_stars_count_down_) {
    the_salvation_from_stars_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        TheSalvationFromStarsClick();
      }
    }
  }
}

float FallenAngel::GetDamageScale() const {
  float damage_scale = 1;
  if (is_burning_) {
    damage_scale = 1 + 2 * (0.80 - health_);
  }
  if (is_blessed_) {
    damage_scale = damage_scale * 1.5;
  }
  return damage_scale;
}

void FallenAngel::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        if (is_burning_) {
          GenerateBullet<bullet::FireBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
        } else {
          GenerateBullet<bullet::CannonBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity);
        }
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool FallenAngel::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *FallenAngel::UnitName() const {
  return "Fallen Angel Tank";
}

const char *FallenAngel::Author() const {
  return "syl";
}

}  // namespace battle_game::unit
