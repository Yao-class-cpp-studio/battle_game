#pragma once
#include "battle_game/core/object.h"
#include "glm/glm.hpp"

namespace battle_game {

class Unit;

class LifeBar {
 public:
  LifeBar(GameCore *game_core, uint32_t id);

  void SetLength(float new_length);
  void SetOffset(glm::vec2 new_offset);
  void SetFrontColor(glm::vec4 new_color);
  void SetBackgroundColor(glm::vec4 new_color);
  void SetFadeoutColor(glm::vec4 new_color);

  void Show();
  void Hide();
  virtual void Render();

 protected:
  uint32_t id_;
  GameCore *game_core_{nullptr};
  bool display_{true};
  glm::vec2 offset_{};
  float length_{2.4f};
  glm::vec4 front_color_{};
  glm::vec4 background_color_{};
  float fadeout_health_;
  glm::vec4 fadeout_color_{};
};

}  // namespace battle_game