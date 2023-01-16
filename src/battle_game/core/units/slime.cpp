#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"

namespace battle_game::unit {

namespace {
uint32_t slime_model_index = 0xffffffffu;
}  // namespace

Slime::Slime(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~slime_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      std::vector<ObjectVertex> vertices;
      std::vector<uint32_t> indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        vertices.push_back({{sin_theta * 1.0f, cos_theta * 1.0f},
                            {0.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f, 1.0f}});
        indices.push_back(i);
        indices.push_back((i + 1) % precision);
        indices.push_back(precision);
      }
      slime_model_index = mgr->RegisterModel(vertices, indices);
    }
  }
}

Slime::~Slime() {
  if (game_core_) {
    for (auto i = 0; i < death_split_; i++) {
      auto turret_rotation = i * glm::radians(360.0f) / death_split_;
      auto velocity = Rotate(glm::vec2{0.0f, 2.0f}, turret_rotation);
      GenerateBullet<bullet::Splitter>(
          position_ + Rotate({0.0f, 1.2f}, turret_rotation), turret_rotation,
          5 * GetDamageScale(), velocity);
    }
  }
}

void Slime::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture("../../textures/slime.png");
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(0);
}

void Slime::Update() {
  SlimeMove(1.0f, glm::radians(180.0f));
  Fire();
}

void Slime::SlimeMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void Slime::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        float turret_rotation;
        auto diff = input_data.mouse_cursor_position - position_;
        if (glm::length(diff) < 1e-4) {
          turret_rotation = rotation_;
        } else {
          turret_rotation = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
        }
        auto velocity = Rotate(glm::vec2{0.0f, 2.0f}, turret_rotation);
        GenerateBullet<bullet::Splitter>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation), turret_rotation,
            5 * GetDamageScale(), velocity);
        game_core_->PushEventDealDamage(id_, id_, 60.0f);
        fire_count_down_ = 2 * kTickPerSecond;  // Fire interval 2 second.
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool Slime::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.5f && position.x < 0.5f && position.y > -0.5f &&
         position.y < 0.5f;
}

float Slime::BasicMaxHealth() const {
  return 3000.0f;
}

const char *Slime::UnitName() const {
  return "Slime";
}

const char *Slime::Author() const {
  return "Zhengxl0";
}
}  // namespace battle_game::unit
