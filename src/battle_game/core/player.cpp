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
      resurrection_count_down_ = kTickPerSecond * 3;  // Respawn after 3 seconds
    }
    resurrection_count_down_--;
    if (!resurrection_count_down_) {
      fire_count_ = kTickPerSecond;
      primary_unit_id_ = game_core_->AllocatePrimaryUnit(id_);
      primary_unit = game_core_->GetUnit(primary_unit_id_);
      last_target_pos_ = primary_unit->GetPosition();
      fixed_pos_ = last_target_pos_ + game_core_->RandomInCircle();
      if (typeid(*primary_unit) == typeid(unit::InfernoTank))
        input_data_.key_down[GLFW_KEY_E] = true;
      else
        input_data_.key_down[GLFW_KEY_E] = false;
    }
  } else {
    UpdateLogic();
  }
}

void AiPlayer::UpdateLogic() {
  auto primary_unit = game_core_->GetUnit(primary_unit_id_);
  auto pos = primary_unit->GetPosition();
  auto primary_rotation = primary_unit->GetRotation();

  auto &units = game_core_->GetUnits();
  glm::vec2 cloest_pos = pos;
  glm::vec2 target_pos;
  float target_diff = 2048.0f;
  float best_diff = 2048.0f;
  bool find_target = false;
  for (auto &unit : units) {
    if (unit.second->GetPlayerId() == id_) {
      continue;
    }
    auto unit_pos = unit.second->GetPosition();
    auto diff = pos - unit_pos;
    if (glm::length(diff) < best_diff) {
      best_diff = glm::length(diff);
      find_target = true;
      cloest_pos = unit_pos;
    }
    diff = last_target_pos_ - unit_pos;
    if (glm::length(diff) < best_diff) {
      target_diff = glm::length(diff);
      target_pos = unit_pos;
    }
  }
  if (request_change_target_ > 0) {
    request_change_target_--;
  }
  input_data_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] = false;
  if (find_target) {
    if (cloest_pos != target_pos) {
      request_change_target_ += 4;
      if (request_change_target_ > 40) {
        target_pos = cloest_pos;
        request_change_target_ = 0;
      }
    }
    auto fix_vec = target_pos - fixed_pos_;
    auto len = glm::length(fix_vec);
    if (len > 1.0f) {
      fix_vec += sqrt(len) * game_core_->RandomInCircle();
    } else {
      fix_vec += game_core_->RandomInCircle();
    }
    fixed_pos_ += fix_vec * 0.06f + (target_pos - last_target_pos_);
    last_target_pos_ = target_pos;

    if (fire_count_ == 0 && glm::length(fixed_pos_ - target_pos) < 1.2f) {
      fire_count_ = 1.8 * kTickPerSecond;
      input_data_.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] = true;
    } else {
      if (fire_count_ > 0)
        fire_count_--;
    }
  }
  input_data_.mouse_cursor_position = fixed_pos_;

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
      auto cos = glm::cos(primary_rotation - bullet_rotation);
      if (cos < 0)
        forward_turn = !forward_turn;
      if (std::fabs(cos) < 0.4)
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

}  // namespace battle_game