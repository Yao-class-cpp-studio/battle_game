#include "buff_generator.h"

#include "battle_game/core/game_core.h"

namespace battle_game::unit {

BuffGenerator::BuffGenerator(GameCore *game_core,
                             uint32_t id,
                             uint32_t player_id)
    : Unit(game_core, id, player_id) {
}

void BuffGenerator::Update() {
  if (!(game_core_->GetUnit(buff_id_))) {
    BuffGenerate();
  }
}

void BuffGenerator::Render() {
}

bool BuffGenerator::IsHit(glm::vec2 position) const {
  return false;
}

void BuffGenerator::RenderLifeBar() {
}

void BuffGenerator::BuffGenerate() {
  if (buff_count_down_) {
    buff_count_down_--;
  } else {
    buff_id_ = game_core_->AddUnit<unit::BloodReturn>(0);
    // There should be a GetBoundaryLow() and a GetBoudaryHigh() function in
    // GameCore...
    float x = game_core_->RandomFloat() * 20.0f - 10.0f;
    float y = game_core_->RandomFloat() * 20.0f - 10.0f;
    game_core_->GetUnit(buff_id_)->SetPosition(glm::vec2{x, y});
    buff_count_down_ = 20 * kTickPerSecond;
  }
}

const char *BuffGenerator::UnitName() const {
  return "Buff Generator";
}

const char *BuffGenerator::Author() const {
  return "CoffeePot1206";
}

}  // namespace battle_game::unit
