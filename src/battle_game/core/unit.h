#pragma once
#include <list>

#include "battle_game/core/object.h"
#include "glm/glm.hpp"

namespace battle_game {

class Bullet;
class Effect;

class Unit : public Object {
 public:
  class Status {
   public:
    GameCore *const game_core_;
    Unit *const unit_;
    float attack_;
    float defence_;
    float speed_;
    std::function<bool(glm::vec2)> is_hit_;
    glm::vec2 color_;

    float GetHealth() const {
      return health_;
    }
    float GetMaxHealth() const {
      return max_health_;
    }

    Status(GameCore *game_core,
           Unit *unit,
           std::function<bool(glm::vec2)> is_hit,
           float max_health,
           float health,
           float attack,
           float defence,
           float speed);

    void Initialization();
    void HealthChange(uint32_t, float);

   private:
    float max_health_;
    float health_;
    const float base_attack_;
    const float base_defence_;
    const float base_speed_;
    const std::function<bool(glm::vec2)> base_is_hit_;
  };

  Unit(
      GameCore *game_core,
      uint32_t id,
      uint32_t player_id,
      std::function<bool(glm::vec2)> is_hit =
          [](glm::vec2 position) { return false; },
      float max_health = 100.0f,
      float health = 1.0f,
      float attack = 1.0f,
      float defence = .0f,
      float speed = 1.0f);

  uint32_t &GetPlayerId() {
    return player_id_;
  }
  [[nodiscard]] uint32_t GetPlayerId() const {
    return player_id_;
  }
  void SetPosition(glm::vec2 position);
  void SetRotation(float rotation);

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
  virtual void RenderLifeBar();

  /*
   * This virtual function is used to render some extra helpers, such as
   * predicted trajectory of the bullet the unit will shoot, and etc., only
   * in the first-person perspective.
   * */
  virtual void RenderHelper();

  /*
   * This virtual function is used to check whether a bullet at the position
   * have hit the unit. If the position is inside the unit area, then return
   * true, otherwise return false.
   * */
  [[nodiscard]] virtual bool IsHit(glm::vec2 position) const {
    return status_.is_hit_(position);
  }

  template <class BulletType, class... Args>
  void GenerateBullet(glm::vec2 position,
                      float rotation,
                      float damage_scale = 1.0f,
                      Args... args);

  [[nodiscard]] virtual const char *UnitName() const;
  [[nodiscard]] virtual const char *Author() const;
  const std::vector<Skill> &GetSkills() const {
    return skills_;
  }

  virtual void Damage(uint32_t src_unit_id, float damage) {
    if (damage < .0f)
      return;
    status_.HealthChange(src_unit_id, -damage * GetDamageReduction());
  }

  void PushEffect(Effect *x) {
    effect_.push_back(x);
  }

  const std::list<Effect *> &GetEffect() const {
    return effect_;
  }

  void UpdateStatus();
  void RemoveEffect();

  [[nodiscard]] float GetMaxHealth() const {
    return status_.GetMaxHealth();
  }

  [[nodiscard]] virtual float GetDamageScale() const {
    return GetAttack();
  }

  [[nodiscard]] virtual float GetDamageReduction() const {
    return 1.0f / (1.0f + GEtDefence());
  }

  [[nodiscard]] virtual float GetSpeedScale() const {
    return GetSpeed();
  }

  /*
   * Health value is in range [0, 1], represents the remaining health in ratio
   * form. GetHealth() * GetMaxHealth() represent true remaining health of the
   * unit.
   * */

  [[nodiscard]] float GetHealth() const {
    return status_.GetHealth();
  }
  [[nodiscard]] float GetAttack() const {
    return status_.attack_;
  }
  [[nodiscard]] float GEtDefence() const {
    return status_.defence_;
  }
  [[nodiscard]] float GetSpeed() const {
    return status_.speed_;
  }

 protected:
  uint32_t player_id_{};
  std::vector<Skill> skills_;
  bool lifebar_display_{true};
  glm::vec2 lifebar_offset_{};
  float lifebar_length_{2.4f};
  glm::vec4 front_lifebar_color_{};
  glm::vec4 background_lifebar_color_{};
  glm::vec4 fadeout_lifebar_color_{};

 private:
  float fadeout_health_;
  Status status_;
  std::list<Effect *> effect_;
};

}  // namespace battle_game

// add something to pull