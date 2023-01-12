#include "battle_game/core/obstacle.h"

#include "battle_game/core/game_core.h"
namespace battle_game {
namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace
Obstacle::Obstacle(GameCore *game_core,
                   uint32_t id,
                   glm::vec2 position,
                   float rotation)
    : Object(game_core, id, position, rotation) {
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
float Obstacle::BasicMaxHealth() const {
  return 100.0f;
}

float Obstacle::GetHealthScale() const {
  return 1.0f;
}

void Obstacle::SetLifeBarLength(float new_length) {
  lifebar_length_ = std::min(new_length, 0.0f);
}
void Obstacle::SetLifeBarOffset(glm::vec2 new_offset) {
  lifebar_offset_ = new_offset;
}
void Obstacle::SetLifeBarFrontColor(glm::vec4 new_color) {
  front_lifebar_color_ = new_color;
}
void Obstacle::SetLifeBarBackgroundColor(glm::vec4 new_color) {
  background_lifebar_color_ = new_color;
}
void Obstacle::SetLifeBarFadeoutColor(glm::vec4 new_color) {
  fadeout_lifebar_color_ = new_color;
}
float Obstacle::GetLifeBarLength() {
  return lifebar_length_;
}
glm::vec2 Obstacle::GetLifeBarOffset() {
  return lifebar_offset_;
}
glm::vec4 Obstacle::GetLifeBarFrontColor() {
  return front_lifebar_color_;
}
glm::vec4 Obstacle::GetLifeBarBackgroundColor() {
  return background_lifebar_color_;
}
glm::vec4 Obstacle::GetLifeBarFadeoutColor() {
  return fadeout_lifebar_color_;
}

void Obstacle::ShowLifeBar() {
  lifebar_display_ = true;
}
void Obstacle::HideLifeBar() {
  lifebar_display_ = false;
}
void Obstacle::SetDestructible() {
  is_destructible_ = true;
}
void Obstacle::SetUndestructible() {
  is_destructible_ = false;
}
bool Obstacle::IsDestructible() {
  return is_destructible_;
}

void Obstacle::RenderLifeBar() {
  if (lifebar_display_ && is_destructible_) {
    auto parent_obstacle = game_core_->GetObstacle(id_);
    auto pos = parent_obstacle->GetPosition() + lifebar_offset_;
    auto health = parent_obstacle->GetHealth();
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
void Obstacle::Update() {
}

void Obstacle::Render() {
}
}  // namespace battle_game
