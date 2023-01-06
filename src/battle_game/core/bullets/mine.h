#pragma once
#include "battle_game/core/bullet.h"
#include "battle_game/core/game_core.h"

namespace battle_game::bullet {
  class Mine : public Bullet {
  public:
    Mine(GameCore* core,
      uint32_t id,
      uint32_t unit_id,
      uint32_t player_id,
      glm::vec2 position,
      float rotation,
      float damage_scale,
      uint32_t velocity);
    ~Mine() override;
    void Render() override;
    void Update() override;

  private:
    uint32_t velocity_{ 0 };
    uint32_t ready_count_down_{ 0 };
  };
}  // namespace battle_game::bullet