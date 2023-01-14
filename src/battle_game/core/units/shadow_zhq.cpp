#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/units/double_scatter_tank.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

Shadow::Shadow(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "Rocket";
  temp.description = "Little Bullet";
  temp.time_remain = 0;
  temp.time_total = 0;
  temp.bullet_type = 1;
  temp.bullet_total_number = 1;
  temp.type = B;
  skills_.push_back(temp);
}

void Shadow::Render() {
  Tank::Render();
}

void Shadow::Update() {
  Fire();
}

void Shadow::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (game_core_->GetUnits().size() > 1) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Rocket>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = 300;  // Fire interval 1 second.
      }
    }
  }
}

bool Shadow::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *Shadow::UnitName() const {
  return "Shadow";
}

const char *Shadow::Author() const {
  return "zhqwq";
}
}  // namespace battle_game::unit
