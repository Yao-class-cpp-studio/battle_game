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
          uint32_t type,
          float scale);
  void Render() override;
  void Update() override;

 private:
  uint32_t type_;
  std::vector<std::string> typenames_{"speed", "damage", "shield", "fire",
                                      "heal"};
  float scale_;
  float lifetime_;
  float strength_{1.0f};
};
}  // namespace battle_game::bullet
