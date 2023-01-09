#pragma once
#include "battle_game/core/bullet.h"
namespace battle_game::bullet {
class DropBox : public Bullet {
 public:
  DropBox(GameCore *core,
          uint32_t id,
          uint32_t unit_id,
          uint32_t player_id,
          glm::vec2 position,
          float rotation,
          float damage_scale,
          const std::string effect,
          float scale);
  void Render() override;
  void Update() override;

 private:
  const std::string effect_;
  float scale_;
  float lifetime_;
  float strength_{1.0f};
  uint32_t is_got_{0};
};
}  // namespace battle_game::bullet
