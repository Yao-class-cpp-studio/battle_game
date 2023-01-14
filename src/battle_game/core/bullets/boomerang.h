#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Boomerang : public Bullet {
 public:
  Boomerang(GameCore *core,
            uint32_t id,
            uint32_t unit_id,
            uint32_t player_id,
            glm::vec2 position,
            float rotation,
            float damage_scale,
            glm::vec2 velocity);
  ~Boomerang() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  int time_{0};
  int hit_{0};
  bool return_{false};
};
}  // namespace battle_game::bullet
