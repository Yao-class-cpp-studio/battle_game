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
  const auto pos = primary_unit->GetPosition();
  const auto primary_rotation = primary_unit->GetRotation();
  if (!primary_unit) {
    if (!resurrection_count_down_) {
      resurrection_count_down_ = kTickPerSecond * 2;  // Respawn after 2 seconds
    }
    resurrection_count_down_--;
    if (!resurrection_count_down_) {
      primary_unit_id_ = game_core_->AllocatePrimaryUnit(id_);
    }
  } else {
    // shot to the cloest enemy
    if (!fire_count_down_) {
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
      if (should_fire) {
        fire_count_down_ = 2 * kTickPerSecond;
        input_data_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] = true;
        input_data_.mouse_cursor_position = target_pos;
      }

    } else {
      fire_count_down_--;
    }
    auto &bullets = game_core_->GetBullets();
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

      auto l = glm::length(diff) *
               std::fabs(glm::sin(rel_rotation - bullet_rotation));
      if (l < 1.0f) {
        if (glm::sin(primary_rotation - bullet_rotation) > 0) {
          input_data_.key_down[GLFW_KEY_W] = true;
        } else {
          input_data_.key_down[GLFW_KEY_S] = true;
        }
        break;
      }
    }
  }
}

}  // namespace battle_game