#include "battle_game/core/game_core.h"

namespace battle_game {

namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

Unit::Status::Status(GameCore *game_core,
                     Unit *unit,
                     std::function<bool(glm::vec2)> is_hit,
                     float max_health,
                     float health,
                     float attack,
                     float defence,
                     float speed)
    : game_core_{game_core},
      unit_{unit},
      base_is_hit_{is_hit},
      max_health_{max_health},
      health_{health},
      base_attack_{attack},
      base_defence_{defence},
      base_speed_(speed) {
}

void Unit::Status::HealthChange(uint32_t src_unit_id, float change) {
  if ((health_ += change / max_health_) <= 0.0f)
    game_core_->PushEventKillUnit(unit_->GetId(), src_unit_id);
  if (health_ > 1.0f)
    health_ = 1.0f;
}

void Unit::Status::Initialization() {
  attack_ = base_attack_;
  defence_ = base_defence_;
  speed_ = base_speed_;
  is_hit_ = base_is_hit_;
}

Unit::Unit(GameCore *game_core,
           uint32_t id,
           uint32_t player_id,
           std::function<bool(glm::vec2)> is_hit,
           float max_health,
           float health,
           float attack,
           float defence,
           float speed)
    : Object(game_core, id),
      status_(game_core,
              this,
              is_hit,
              max_health,
              health,
              attack,
              defence,
              speed) {
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

void Unit::UpdateStatus() {
  status_.Initialization();
  for (auto i : effect_)
    i->Influence(status_), i->TickPass();
  status_.attack_ = std::max(status_.attack_, .0f);
  status_.defence_ = std::max(status_.defence_, .0f);
  status_.speed_ = std::max(status_.speed_, .0f);
}

void Unit::RemoveEffect() {
  effect_.remove_if([](Effect *effect) {
    return effect->ShouldRemove() ? delete effect, true : false;
  });
}

const char *Unit::UnitName() const {
  return "Unknown Unit";
}
const char *Unit::Author() const {
  return "Unknown Author";
}
}  // namespace battle_game
