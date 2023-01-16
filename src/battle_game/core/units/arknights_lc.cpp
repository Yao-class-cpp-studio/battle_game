#include "arknights_lc.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/core/obstacles/block.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

Arknights::Arknights(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  Skill temp;
  temp.name = "operators";
  temp.description = "release an obstacle";
  temp.time_remain = 200;
  temp.time_total = 500;
  temp.type = E;
  skills_.push_back(temp);
}

void Arknights::Render() {
  Tank::Render();
}

void Arknights::Update() {
  ArknightsMove(3.0f);
  TurretRotate();
  Fire();
  operators();
}

void Arknights::ArknightsMove(float move_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W] || input_data.key_down[GLFW_KEY_UP]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S] || input_data.key_down[GLFW_KEY_DOWN]) {
      offset.y -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_A] || input_data.key_down[GLFW_KEY_LEFT]) {
      offset.x -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D] ||
        input_data.key_down[GLFW_KEY_RIGHT]) {
      offset.x += 1.0f;
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position = position_ + offset;
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
  }
}

float Arknights::GetSpeedScale() const {
  return 0.9f;
}

float Arknights::GetHealthScale() const {
  return 2.5f;
}

void Arknights::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void Arknights::Fire() {
  Tank::Fire();
}

bool Arknights::IsHit(glm::vec2 position) const {
  return Tank::IsHit(position);
}

void Arknights::operators() {
  skills_[0].time_remain = operator_count_down_;
  if (operator_count_down_) {
    operator_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data1 = player->GetInputData();
      if (input_data1.key_down[GLFW_KEY_E]) {
        auto &input_data2 = player->GetInputData();
        if (input_data2.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
          auto diff = input_data2.mouse_cursor_position - position_;
          if (glm::length(diff) < 1) {
            return;
          } else {
            auto new_position = position_ + diff;
            float rotation = 0.0f;
            game_core_->AddObstacle<battle_game::obstacle::Block>(new_position,
                                                                  rotation);
            operator_count_down_ = 6 * kTickPerSecond;
          }
        }
      }
    }
  }
}

const char *Arknights::UnitName() const {
  return "[kokodayo] Arknights";
}

const char *Arknights::Author() const {
  return "liuchuan22";
}
}  // namespace battle_game::unit
