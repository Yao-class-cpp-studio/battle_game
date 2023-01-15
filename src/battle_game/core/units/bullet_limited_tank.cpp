#include "battle_game/core/units/bullet_limited_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

BulletLimitedTank::BulletLimitedTank(GameCore *game_core,
                                     uint32_t id,
                                     uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "弹药余量：" + std::to_string(bullet_remaining_) + "/" +
              std::to_string(bullet_limit_) + "，按R进行换弹";
  temp.time_remain = 0;
  temp.time_total = 360;
  temp.type = R;
  // temp.function = SKILL_ADD_FUNCTION(BulletLimitedTank::ReloadClick);*/
  skills_.push_back(temp);
}

void BulletLimitedTank::Render() {
  Tank::Render();
}

void BulletLimitedTank::ReloadClick() {
  skills_[0].name = "换弹中";
  skills_[0].time_remain = 360;
  reload_count_down_ = 6 * kTickPerSecond;
}
void BulletLimitedTank::Reload() {
  skills_[0].time_remain = reload_count_down_;
  if (reload_count_down_) {
    reload_count_down_--;
    if (reload_count_down_ == 0) {
      bullet_remaining_ = bullet_limit_;
      skills_[0].name = "弹药余量：" + std::to_string(bullet_remaining_) + "/" +
                        std::to_string(bullet_limit_) + "，按R进行换弹";
    }
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_R]) {
        ReloadClick();
      }
    }
  }
}
void BulletLimitedTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Reload();
  Fire();
}
void BulletLimitedTank::Fire() {
  if (fire_count_down_ == 0 && reload_count_down_ == 0 && bullet_remaining_) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        --bullet_remaining_;
        skills_[0].name = "弹药余量：" + std::to_string(bullet_remaining_) +
                          "/" + std::to_string(bullet_limit_) + "，按R进行换弹";
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool BulletLimitedTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *BulletLimitedTank::UnitName() const {
  return "Bullet Limited Tank";
}

const char *BulletLimitedTank::Author() const {
  return "QAQAutoMaton";
}
}  // namespace battle_game::unit
