#include "battle_game/core/units/rebounding_sample_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

ReboundingSampleTank::ReboundingSampleTank(GameCore *game_core,
                                           uint32_t id,
                                           uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Rebounding Bullet";
  temp.description =
      "Bullets that can rebound at most 2 times. Cause 10 points of damage to "
      "enemies.";
  temp.time_remain = 0;
  temp.time_total = kTickPerSecond;
  temp.bullet_type = 1;
  temp.bullet_total_number = 1;
  temp.type = B;
  skills_.push_back(temp);
}

void ReboundingSampleTank::Render() {
  Tank::Render();
}

void ReboundingSampleTank::Update() {
  Tank::TankMove(3.0f, glm::radians(180.0f));
  Tank::TurretRotate();
  Fire();
}

void ReboundingSampleTank::Fire() {
  if (skills_[0].time_remain) {
    skills_[0].time_remain--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        for (int i = -2; i <= 2; ++i) {
          auto offset = glm::radians(30.0f * i);
          velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_ + offset);
          GenerateBullet<bullet::ReboundingBall>(
              position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
              turret_rotation_, GetDamageScale(), velocity,
              2);  // Rebound 2 times.
        }
        skills_[0].time_remain =
            skills_[0].time_total;  // Fire interval 1 second.
      }
    }
  }
}

const char *ReboundingSampleTank::UnitName() const {
  return "Rebounding Sample Tank";
}

const char *ReboundingSampleTank::Author() const {
  return "JasonL";
}
}  // namespace battle_game::unit
