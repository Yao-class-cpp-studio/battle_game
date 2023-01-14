#pragma once
#include "battle_game/core/bullet.h"
namespace battle_game::bullet {
class AirborneBomb : public Bullet {
 public:
  AirborneBomb(GameCore *core,
              uint32_t id,
              uint32_t unit_id,
              uint32_t player_id,
              glm::vec2 position,
              float rotation,
              float damage_scale,
              glm::vec2 velocity);
  ~AirborneBomb() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float rotation_;
  uint32_t duration = 0;
};
}  // namespace battle_game::bullet
