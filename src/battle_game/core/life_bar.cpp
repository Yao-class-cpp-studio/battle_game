#include "battle_game/core/life_bar.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/unit.h"
namespace battle_game {

namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

LifeBar::LifeBar(GameCore *game_core, uint32_t id)
    : id_(id), game_core_(game_core) {
  offset_ = {0.0f, 1.0f};
  background_color_ = {1.0f, 0.0f, 0.0f, 0.9f};
  front_color_ = {0.0f, 1.0f, 0.0f, 0.9f};
  if (!~life_bar_model_index) {
    auto mgr = AssetsManager::GetInstance();
    life_bar_model_index = mgr->RegisterModel(
        {{{-0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{-0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
        {0, 1, 2, 1, 2, 3});
  }
}

void LifeBar::SetLength(float new_length) {
  length_ = std::min(new_length, 0.0f);
}
void LifeBar::SetOffset(glm::vec2 new_offset) {
  offset_ = new_offset;
}
void LifeBar::SetFrontColor(glm::vec4 new_color) {
  front_color_ = new_color;
}
void LifeBar::SetBackgroundColor(glm::vec4 new_color) {
  background_color_ = new_color;
}
void LifeBar::Show() {
  display_ = true;
}
void LifeBar::Hide() {
  display_ = false;
}

void LifeBar::Render() {
  if (display_) {
    auto parent_unit = game_core_->GetUnit(id_);
    auto pos = parent_unit->GetPosition() + offset_;
    auto health = parent_unit->GetHealth();
    SetTransformation(pos, 0.0f, {length_, 1.0f});
    SetColor(background_color_);
    SetTexture(0);
    DrawModel(life_bar_model_index);
    glm::vec2 shift = {(float)length_ * (1 - health) / 2, 0.0f};
    SetTransformation(pos - shift, 0.0f, {length_ * health, 1.0f});
    SetColor(front_color_);
    DrawModel(life_bar_model_index);
  }
}

}  // namespace battle_game
