#include "battle_game/core/game_core.h"

namespace battle_game::bullet {
AccumulatorBullet::AccumulatorBullet(GameCore *core,
                                     uint32_t id,
                                     uint32_t unit_id,
                                     uint32_t player_id,
                                     glm::vec2 position,
                                     float rotation,
                                     float damage_scale,
                                     glm::vec2 velocity)
    : CannonBall(core,
                 id,
                 unit_id,
                 player_id,
                 position,
                 rotation,
                 damage_scale * 2,
                 velocity) {
}

void AccumulatorBullet::Render() {
  SetTransformation(position_, .0f, glm::vec2{damage_scale_} * 0.25f);
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/accumulator_bullet.png");
  DrawModel(0);
}

}  // namespace battle_game::bullet
