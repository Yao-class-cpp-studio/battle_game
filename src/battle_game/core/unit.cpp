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
  state_ = {{Immune, 2 * kTickPerSecond},
            {SpeedUp, 0},
            {SpeedDown, 0},
            {OnFire, 0},
            {Weak, 0},
            {Strong, 0},
            {Stunning, 0}};  // Immune to all damage for 2 seconds.
}

bool Unit::IsBuffed(Buff b) const {
  if (state_[b])
    return true;
  return false;
}

void Unit::SetBuff(Buff b, uint32_t time) {
  state_[b] = time;
}

void Unit::UpdateState() {
  if (IsBuffed(Immune))
    state_[Immune]--;
  if (IsBuffed(SpeedUp))
    state_[SpeedUp]--;
  if (IsBuffed(SpeedDown))
    state_[SpeedDown]--;
  if (IsBuffed(OnFire)) {
    game_core_->PushEventDealDamage(id_, id_, 0.05);
    state_[OnFire]--;
  }
  if (IsBuffed(Weak))
    state_[Weak]--;
  if (IsBuffed(Strong))
    state_[Strong]--;
  if (IsBuffed(Stunning))
    state_[Stunning]--;
}

void Unit::SetPosition(glm::vec2 position) {
  position_ = position;
}

void Unit::SetRotation(float rotation) {
  rotation_ = rotation;
}

float Unit::GetSpeedScale() const {
  float s = 1.0f;
  if (IsBuffed(SpeedUp))
    s *= 2;
  if (IsBuffed(SpeedDown))
    s *= 0.5;
  return s;
}

float Unit::GetDamageScale() const {
  float s = 1.0f;
  if (IsBuffed(Strong))
    s *= 2;
  if (IsBuffed(Weak))
    s *= 0.5;
  return s;
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
