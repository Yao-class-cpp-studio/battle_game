#pragma once
#include "battle_game/core/object.h"
#include "glm/glm.hpp"

namespace battle_game {
class Obstacle : public Object {
 public:
  Obstacle(GameCore *game_core,
           uint32_t id,
           glm::vec2 position,
           float rotation = 0.0f);
  [[nodiscard]] virtual bool IsBlocked(glm::vec2 p) const = 0;
  void Update() override;
  void Render() override;
  virtual std::pair<glm::vec2, glm::vec2> GetSurfaceNormal(glm::vec2 origin,
                                                           glm::vec2 terminus) {
    return std::make_pair(glm::vec2(0, 0), glm::vec2(0, 0));
  }
  [[nodiscard]] virtual float BasicMaxHealth() const;
  [[nodiscard]] virtual float GetHealthScale() const;
  [[nodiscard]] virtual float GetMaxHealth() const {
    return std::max(GetHealthScale() * BasicMaxHealth(), 1.0f);
  }
  [[nodiscard]] float GetHealth() const {
    return health_;
  }
  void SetHealth(float new_health) {
    health_ = std::clamp(new_health, 0.0f, 1.0f);
  }

  void SetLifeBarLength(float new_length);
  void SetLifeBarOffset(glm::vec2 new_offset);
  void SetLifeBarFrontColor(glm::vec4 new_color);
  void SetLifeBarBackgroundColor(glm::vec4 new_color);
  void SetLifeBarFadeoutColor(glm::vec4 new_color);
  [[nodiscard]] float GetLifeBarLength();
  [[nodiscard]] glm::vec2 GetLifeBarOffset();
  [[nodiscard]] glm::vec4 GetLifeBarFrontColor();
  [[nodiscard]] glm::vec4 GetLifeBarBackgroundColor();
  [[nodiscard]] glm::vec4 GetLifeBarFadeoutColor();

  void ShowLifeBar();
  void HideLifeBar();
  void SetDestructible();
  void SetUndestructible();
  bool IsDestructible();
  virtual void RenderLifeBar();

 protected:
  float health_{1.0f};
  bool is_destructible_{false};
  bool lifebar_display_{true};
  glm::vec2 lifebar_offset_{};
  float lifebar_length_{2.4f};
  glm::vec4 front_lifebar_color_{};
  glm::vec4 background_lifebar_color_{};
  glm::vec4 fadeout_lifebar_color_{};

 private:
  float fadeout_health_;
};
}  // namespace battle_game
