#include "battle_game/core/unit.h"

#include "battle_game/core/game_core.h"

namespace battle_game {
Unit::Unit(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Object(game_core, id) {
  player_id_ = player_id;
}

void Unit::SetPosition(glm::vec2 position) {
  position_ = position;
}

void Unit::SetRotation(float rotation) {
  rotation_ = rotation;
}

float Unit::GetSpeedScale() const {
  return 1.0f;
}

float Unit::GetDamageScale() const {
  return 1.0f;
}

float Unit::BasicMaxHealth() const {
  return 100.0f;
}

float Unit::GetHealthScale() const {
  return 1.0f;
}

std::vector<glm::vec2> Unit::GetVertices() const {
  return {};
}

const char *Unit::UnitName() const {
  return "Unknown Unit";
}
const char *Unit::Author() const {
  return "Unknown Author";
}
}  // namespace battle_game
