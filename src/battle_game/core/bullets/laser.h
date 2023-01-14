#pragma once
#include "battle_game/core/bullet.h"
#define TimeLimit 2000
namespace battle_game::bullet {
class Laser : public Bullet {
 public:
  Laser(GameCore *core,
        uint32_t id,
        uint32_t unit_id,
        uint32_t player_id,
        glm::vec2 position,
        float rotation,
        float damage_scale,
        glm::vec2 velocity,
        int LightIndex);
  ~Laser() override;
  void Render() override;
  void Update() override;

 private:
  void update_color(float &a);
  float angle();
  float Angle_();        // ComboTime, LightIndex_
  float Transparent_();  // ComboTime
  float Width_();        // ComboTime
  float White_(int Distance);
  int ComboTime;
  int LightIndex_;
  glm::vec2 Original_velocity_{};
  glm::vec2 velocity_{};
  glm::vec2 end_position_{};
  glm::vec4 color_{};
};
}  // namespace battle_game::bullet
