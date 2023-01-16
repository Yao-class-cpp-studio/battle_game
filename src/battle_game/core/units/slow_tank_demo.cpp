#include "slow_tank_demo.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

SlowTank::SlowTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {

}

void SlowTank::Render() {
  Tank::Render();
}

void SlowTank::Update() {
  int move_speed = 3.0f;
  if (slowtime_) {
    slowtime_--;
    move_speed *= 0.5f;
  }
  if (speeduptime_) {
    speeduptime_--;
    move_speed *= 2.0f;
  }
  TankMove(move_speed, glm::radians(180.0f));
  TurretRotate();
  Fire();
  if (GetHealth() < 0.25f)
    Smoke();
}

void SlowTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::SlowCannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

void SlowTank::TurrentRotate() {
  Tank::TurrentRotate();
}

const char *SlowTank::UnitName() const {
  return "Slow Tank(demo)";
}

const char *SlowTank::Author() const {
  return "Roy Wang";
}
}  // namespace battle_game::unit
