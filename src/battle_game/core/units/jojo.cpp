#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "jojo.h"

namespace battle_game::unit {

namespace {
uint32_t jojo_body_model_index = 0xffffffffu;
uint32_t jojo_turret_model_index = 0xffffffffu;
}  // namespace

JoJo::JoJo(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~jojo_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* JoJo Body */
      jojo_body_model_index = mgr->RegisterModel(
          {
              {{-0.76f/20-0.20f, 26.75f/20} ,{0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-8.32f / 20 - 0.20f, 15.72f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-3.01f / 20 - 0.20f, 5.75f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-16.72f / 20 - 0.20f, -9.67f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-19.00f / 20 - 0.20f, -28.86f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-15.75f / 20 - 0.20f, -28.86f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-11.79f / 20 - 0.20f, -14.29f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{2.78f / 20 - 0.20f, -8.05f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{12.62f / 20 - 0.20f, -15.51f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{19.12f / 20 - 0.20f, -30.44f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{23.86f / 20 - 0.20f, -30.44f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{19.82f / 20 - 0.20f, -14.46f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{13.59f / 20 - 0.20f, 0.03f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{19.82f / 20 - 0.20f, 5.75f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{19.91f / 20 - 0.20f, 13.64f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{17.89f / 20 - 0.20f, 20.49f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{21.75f / 20 - 0.20f, 23.30f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{22.89f / 20 - 0.20f, 27.25f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}}, 
              {{19.03f / 20 - 0.20f, 31.47f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}}, 
              {{13.59f / 20 - 0.20f, 32.09f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}}, 
              {{10.32f / 20 - 0.20f, 27.05f / 20},
               {0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f, 1.0f}},
              {{7.23f / 20 - 0.20f, 27.46f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{5.36f / 20 - 0.20f, 27.29f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{8.52f / 20 - 0.20f, 17.33f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{7.23f / 20 - 0.20f, 15.72f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{8.86f / 20 - 0.20f, 15.06f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{4.37f / 20 - 0.20f, 3.83f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{6.50f / 20 - 0.20f, 3.83f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{20.36f / 20 - 0.20f, 9.7f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
              {{19.82f / 20 - 0.20f, 5.75f / 20},
               {0.0f, 0.0f},
               {0.5f, 0.5f, 0.5f, 1.0f}},
          },
          {0,1,2,2,3,6,3,6,4,4,6,5,2,7,6,2,7,12,7,12,8,12,8,11,8,11,9,11,9,10,12,13,14,14,15,20,0,20,14,14,12,0,0,2,12,20,15,16,20,16,17,20,17,18,20,18,19,21,22,23,22,23,24,24,25,26,26,25,27,23,25,28,25,28,29,23,24,25});
    }
  
    {
      /* JoJo Turret */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      turret_vertices.push_back(
          {{-0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices = {0};
      jojo_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
}

void JoJo::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(jojo_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(jojo_turret_model_index);
}

void JoJo::Update() {
  JoJoMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void JoJo::JoJoMove(float move_speed, float rotate_angular_speed) {
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
    if (input_data.key_down[GLFW_KEY_A]) {
      offset.x -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      offset.x += 1.0f;
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

void JoJo::TurretRotate() {
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

void JoJo::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::BasketBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

bool JoJo::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *JoJo::UnitName() const {
  return "Jonathan Joestar";
}

const char *JoJo::Author() const {
  return "JoJobananana";
}
}  // namespace battle_game::unit
