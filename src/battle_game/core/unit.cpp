#include "battle_game/core/unit.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

LifeBar::LifeBar() {
  offset_ = {0.0f, 1.0f};
  background_color_ = {1.0f, 0.0f, 0.0f, 0.9f};
  front_color_ = {0.0f, 1.0f, 0.0f, 0.9f};
  fadeout_color_ = {1.0f, 1.0f, 1.0f, 0.5f};
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

void Unit::SetLifeBarLength(float new_length) {
  lifebar_.length_ = std::min(new_length, 0.0f);
}
void Unit::SetLifeBarOffset(glm::vec2 new_offset) {
  lifebar_.offset_ = new_offset;
}
void Unit::SetLifeBarFrontColor(glm::vec4 new_color) {
  lifebar_.front_color_ = new_color;
}
void Unit::SetLifeBarBackgroundColor(glm::vec4 new_color) {
  lifebar_.background_color_ = new_color;
}
void Unit::SetLifeBarFadeoutColor(glm::vec4 new_color) {
  lifebar_.fadeout_color_ = new_color;
}
float Unit::GetLifeBarLength() {
  return lifebar_.length_;
}
glm::vec2 Unit::GetLifeBarOffset() {
  return lifebar_.offset_;
}
glm::vec4 Unit::GetLifeBarFrontColor() {
  return lifebar_.front_color_;
}
glm::vec4 Unit::GetLifeBarBackgroundColor() {
  return lifebar_.background_color_;
}
glm::vec4 Unit::GetLifeBarFadeoutColor() {
  return lifebar_.fadeout_color_;
}

void Unit::ShowLifeBar() {
  lifebar_.display_ = true;
}
void Unit::HideLifeBar() {
  lifebar_.display_ = false;
}

void Unit::RenderLifeBar() {
  if (lifebar_.display_) {
    auto parent_unit = game_core_->GetUnit(id_);
    auto pos = parent_unit->GetPosition() + lifebar_.offset_;
    auto health = parent_unit->GetHealth();
    SetTransformation(pos, 0.0f, {lifebar_.length_, 1.0f});
    SetColor(lifebar_.background_color_);
    SetTexture(0);
    DrawModel(life_bar_model_index);
    glm::vec2 shift = {(float)lifebar_.length_ * (1 - health) / 2, 0.0f};
    SetTransformation(pos - shift, 0.0f, {lifebar_.length_ * health, 1.0f});
    SetColor(lifebar_.front_color_);
    DrawModel(life_bar_model_index);
    if (std::fabs(health - lifebar_.fadeout_health_) >= 0.01f) {
      lifebar_.fadeout_health_ =
          health + (lifebar_.fadeout_health_ - health) * 0.93;
      shift = {lifebar_.length_ * (health + lifebar_.fadeout_health_ - 1) / 2,
               0.0f};
      SetTransformation(
          pos + shift, 0.0f,
          {lifebar_.length_ * (health - lifebar_.fadeout_health_), 1.0f});
      SetColor(lifebar_.fadeout_color_);
      DrawModel(life_bar_model_index);
    } else {
      lifebar_.fadeout_health_ = health;
    }
  }
}

const char *Unit::UnitName() const {
  return "Unknown Unit";
}
const char *Unit::Author() const {
  return "Unknown Author";
}
}  // namespace battle_game
