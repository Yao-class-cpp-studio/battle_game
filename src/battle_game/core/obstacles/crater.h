#pragma once
#include "battle_game/core/obstacle.h"


namespace battle_game::obstacle {
class Crater : public Obstacle {
 public:
  Crater(GameCore *game_core,
         uint32_t id,
         glm::vec2 position,
         float rotation = 0.0f,
         glm::vec2 scale = glm::vec2{1.0f, 1.0f});
  void Render() override;
  void Update() override;

 private:
  void Recover();
  uint32_t crater_count_down_{180};
  uint32_t recovery_count_down_{180};
  bool isRecovered_ = false;
  void RecoverRender();
  uint32_t unit_id_{};
  [[nodiscard]] bool IsBlocked(glm::vec2 p) const override;
  glm::vec2 scale_{1.0f};
};
}  // namespace battle_game::obstacle
