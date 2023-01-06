#include "battle_game/core/player.h"

#include "battle_game/core/game_core.h"

namespace battle_game {
Player::Player(GameCore *game_core, uint32_t id)
    : game_core_(game_core), id_(id) {
}

void Player::Update() {
  auto primary_unit = game_core_->GetUnit(primary_unit_id_);
  if (!primary_unit) {
    if (!resurrection_count_down_) {
      resurrection_count_down_ = kTickPerSecond * 5;  // Respawn after 5 seconds
    }
    resurrection_count_down_--;
    if (!resurrection_count_down_) {
      primary_unit_id_ = game_core_->AllocatePrimaryUnit(id_);
    }
  }
}

void AiPlayer::Update() {
  auto primary_unit = game_core_->GetUnit(primary_unit_id_);
  if (!primary_unit) {
    if (!resurrection_count_down_) {
      resurrection_count_down_ = kTickPerSecond * 2;  // Respawn after 2 seconds
    }
    resurrection_count_down_--;
    if (!resurrection_count_down_) {
      fire_count_ = kTickPerSecond;
      primary_unit_id_ = game_core_->AllocatePrimaryUnit(id_);
    }
  } else {
    auto pos = primary_unit->GetPosition();
    auto primary_rotation = primary_unit->GetRotation();
    // shot to the cloest enemy
    auto &units = game_core_->GetUnits();

    glm::vec2 target_pos;
    float best_diff = 2048.0f;
    bool should_fire = false;
    for (auto &unit : units) {
      if (unit.second->GetPlayerId() == id_) {
        continue;
      }
      auto diff = pos - unit.second->GetPosition();
      if (glm::length(diff) < best_diff) {
        best_diff = glm::length(diff);
        should_fire = true;
        target_pos = unit.second->GetPosition();
      }
    }
    input_data_.mouse_cursor_position = target_pos;
    if (should_fire && fire_count_ == 0) {
      fire_count_ = 2 * kTickPerSecond;
      input_data_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] = true;
    } else {
      input_data_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] = false;
      if (fire_count_ > 0)
        fire_count_--;
    }
    auto &bullets = game_core_->GetBullets();
    input_data_.key_down[GLFW_KEY_W] = false;
    input_data_.key_down[GLFW_KEY_S] = false;
    input_data_.key_down[GLFW_KEY_A] = false;
    input_data_.key_down[GLFW_KEY_D] = false;
    for (auto &bullet : bullets) {
      if (bullet.second->GetPlayerId() == id_) {
        continue;
      }
      auto bullet_pos = bullet.second->GetPosition();
      auto diff = pos - bullet_pos;
      float rel_rotation;
      auto distance = glm::length(diff);
      if (distance < 1e-4) {
        break;
      } else {
        rel_rotation = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
      }
      auto bullet_rotation = bullet.second->GetRotation();

      auto l = glm::length(diff) * glm::sin(rel_rotation - bullet_rotation);
      if (std::fabs(l) < 2.2f) {
        bool forward_turn = true;
        if (glm::sin(primary_rotation - bullet_rotation) > 0)
          forward_turn = !forward_turn;
        if (l > 0)
          forward_turn = !forward_turn;
        if (forward_turn) {
          input_data_.key_down[GLFW_KEY_W] = true;
        } else {
          input_data_.key_down[GLFW_KEY_S] = true;
          primary_rotation += glm::radians(180.0f);
        }
        if (glm::cos(primary_rotation - bullet_rotation) < 0)
          forward_turn = !forward_turn;
        if (forward_turn) {
          input_data_.key_down[GLFW_KEY_A] = true;
        } else {
          input_data_.key_down[GLFW_KEY_D] = true;
        }
        break;
      }
    }
  }
}

}  // namespace battle_game