#include "battle_game/core/unit.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

Unit::Unit(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Object(game_core, id) {
  player_id_ = player_id;
  lifebar_offset_ = {0.0f, 1.0f};
  background_lifebar_color_ = {1.0f, 0.0f, 0.0f, 0.9f};
  front_lifebar_color_ = {0.0f, 1.0f, 0.0f, 0.9f};
  fadeout_lifebar_color_ = {1.0f, 1.0f, 1.0f, 0.5f};
  fadeout_health_ = 1;
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

void Unit::SetLifeBarLength(float new_length) {
  lifebar_length_ = std::min(new_length, 0.0f);
}
void Unit::SetLifeBarOffset(glm::vec2 new_offset) {
  lifebar_offset_ = new_offset;
}
void Unit::SetLifeBarFrontColor(glm::vec4 new_color) {
  front_lifebar_color_ = new_color;
}
void Unit::SetLifeBarBackgroundColor(glm::vec4 new_color) {
  background_lifebar_color_ = new_color;
}
void Unit::SetLifeBarFadeoutColor(glm::vec4 new_color) {
  fadeout_lifebar_color_ = new_color;
}
float Unit::GetLifeBarLength() {
  return lifebar_length_;
}
glm::vec2 Unit::GetLifeBarOffset() {
  return lifebar_offset_;
}
glm::vec4 Unit::GetLifeBarFrontColor() {
  return front_lifebar_color_;
}
glm::vec4 Unit::GetLifeBarBackgroundColor() {
  return background_lifebar_color_;
}
glm::vec4 Unit::GetLifeBarFadeoutColor() {
  return fadeout_lifebar_color_;
}

void Unit::ShowLifeBar() {
  lifebar_display_ = true;
}
void Unit::HideLifeBar() {
  lifebar_display_ = false;
}

void Unit::RenderLifeBar() {
  if (lifebar_display_) {
    auto parent_unit = game_core_->GetUnit(id_);
    auto pos = parent_unit->GetPosition() + lifebar_offset_;
    auto health = parent_unit->GetHealth();
    SetTransformation(pos, 0.0f, {lifebar_length_, 1.0f});
    SetColor(background_lifebar_color_);
    SetTexture(0);
    DrawModel(life_bar_model_index);
    glm::vec2 shift = {(float)lifebar_length_ * (1 - health) / 2, 0.0f};
    SetTransformation(pos - shift, 0.0f, {lifebar_length_ * health, 1.0f});
    SetColor(front_lifebar_color_);
    DrawModel(life_bar_model_index);
    if (std::fabs(health - fadeout_health_) >= 0.01f) {
      fadeout_health_ = health + (fadeout_health_ - health) * 0.93;
      shift = {lifebar_length_ * (health + fadeout_health_ - 1) / 2, 0.0f};
      SetTransformation(pos + shift, 0.0f,
                        {lifebar_length_ * (health - fadeout_health_), 1.0f});
      SetColor(fadeout_lifebar_color_);
      DrawModel(life_bar_model_index);
    } else {
      fadeout_health_ = health;
    }
  }
}

void Unit::RenderHelper() {
}

const char *Unit::UnitName() const {
  return "Unknown Unit";
}
const char *Unit::Author() const {
  return "Unknown Author";
}
}  // namespace battle_game
