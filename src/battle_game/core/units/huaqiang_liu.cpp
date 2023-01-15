#include "huaqiang_liu.h"

#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t huaqiang_knife_model_index = 0xffffffffu;
}  // namespace

HuaqLiu::HuaqLiu(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~huaqiang_knife_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Scary Knife */
      huaqiang_knife_model_index = mgr->RegisterModel(
          {
              {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.0f, -0.1f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.2f, -0.1f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.2f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.2f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.2f, -0.2f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, -0.2f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 3, 1, 2, 3, 4, 5, 6, 4, 6, 7});
    }
  }
}

void HuaqLiu::Render() {
  rotation_ = 0.0f;
  battle_game::SetTransformation(position_, rotation_, {0.5f, 1.0f});
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.key_down[GLFW_KEY_A])
      go_left_ = true;
    else if (input_data.key_down[GLFW_KEY_D])
      go_left_ = false;
  }
  if (go_left_) {
    battle_game::SetTexture("../../textures/huaqiang_liu.png");
    knife_offset_ = {-0.4f, -0.4f};
  } else {
    battle_game::SetTexture("../../textures/huaqiang_liu_mirror.png");
    knife_offset_ = {0.4f, -0.5f};
  }
  battle_game::SetColor();
  battle_game::DrawModel();
  battle_game::SetTransformation(position_ + knife_offset_, knife_rotation_,
                                 {knife_length_, 1.0f});
  battle_game::SetTexture();
  battle_game::SetColor();
  battle_game::DrawModel(huaqiang_knife_model_index);
}

void HuaqLiu::Update() {
  Move(3.0f, glm::radians(180.0f));
  KnifeRotate();
  Split();
}

void HuaqLiu::Move(float move_speed, float rotate_angular_speed) {
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
    if (input_data.key_down[GLFW_KEY_D]) {
      offset.x += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_A]) {
      offset.x -= 1.0f;
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
  }
}

void HuaqLiu::KnifeRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      knife_rotation_ = rotation_;
    } else {
      knife_rotation_ = std::atan2(diff.y, diff.x);
    }
  }
}

void HuaqLiu::Split() {
  auto *player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
      auto &units = game_core_->GetUnits();
      for (auto &unit : units) {
        if (unit.first == id_) {
          continue;
        }
        bool ishit = unit.second->IsHit(
            position_ + knife_offset_ +
            knife_length_ * glm::vec2{std::cos(knife_rotation_),
                                      std::sin(knife_rotation_)});
        ishit = ishit ||
                unit.second->IsHit(position_ + knife_offset_ +
                                   0.5f * knife_length_ *
                                       glm::vec2{std::cos(knife_rotation_),
                                                 std::sin(knife_rotation_)});
        if (!has_hit_ && ishit) {
          game_core_->PushEventDealDamage(unit.first, id_,
                                          damage_scale_ * 10.0f);
          has_hit_ = true;
        } else if (!ishit) {
          has_hit_ = false;
        }
      }
    }
  }
}

bool HuaqLiu::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x < 0.5f && position.x > -0.5f && position.y < 1.0f &&
         position.y > 0.0f;
}

const char *HuaqLiu::UnitName() const {
  return "Huaqiang Liu";
}

const char *HuaqLiu::Author() const {
  return "CoffeePot";
}

}  // namespace battle_game::unit
