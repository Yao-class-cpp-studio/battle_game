#include "battle_game/core/game_core.h"

namespace battle_game::unit {

const float accumulate_per_second = .3f;

Accumulator::Accumulator(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id),
      accumulate_per_tick{accumulate_per_second / kTickPerSecond} {
}

void Accumulator::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void Accumulator::RenderHelper() {
  SetTransformation(position_, .0f, glm::vec2{damage_scale_} * 0.5f);
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/accumulator_bullet.png");
  DrawModel(0);
}

void Accumulator::Fire() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT])
      damage_scale_ = std::min(damage_scale_ + accumulate_per_tick, 1.0f);
    else if (damage_scale_ > 1e-4) {
      auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
      GenerateBullet<bullet::AccumulatorBullet>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation_), .0f,
          GetDamageScale() * damage_scale_, velocity);
      damage_scale_ = .0f;
    }
  }
}

const char *Accumulator::UnitName() const {
  return "Accumulator";
}

const char *Accumulator::Author() const {
  return "XieRujian";
}
}  // namespace battle_game::unit
