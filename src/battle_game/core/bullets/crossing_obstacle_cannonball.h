#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class CrossingObstaclesCannonball : public Bullet {
 public:
  CrossingObstaclesCannonball(GameCore *core,
                              uint32_t id,
                              uint32_t unit_id,
                              uint32_t player_id,
                              glm::vec2 position,
                              float rotation,
                              float damage_scale,
                              glm::vec2 velocity);
  ~CrossingObstaclesCannonball() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
};
<<<<<<< HEAD
}  // namespace battle_game::bullet
=======
}  // namespace battle_game::bullet
>>>>>>> efc4d4f1f3df6ae4324ba100220aeb44c9fb690b
