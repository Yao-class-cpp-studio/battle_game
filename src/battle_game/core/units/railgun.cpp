#include "railgun.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {
Railgun::Railgun(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id, 100.0f, 1.0f, 1.0f, 1.5f, 3.0f) {
  Skill temp;
  temp.name = "Thunderbolt";
  temp.description = "Damage on a random unit";
  temp.time_remain = 0;
  temp.time_total = 900;
  temp.type = E;
  skills_.push_back(temp);
}

void Railgun::Render() {
  battle_game::SetTransformation(position_, rotation_, glm::vec2{1.2f});
  battle_game::SetTexture("../../textures/railgun.png");
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(0);
}

void Railgun::Update() {
  GunMove(5.0f, glm::radians(180.0f));
  DirectionRotate();
  Fire();
  Thunderbolt();
}

void Railgun::GunMove(float move_speed, float rotate_angular_speed) {
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

void Railgun::DirectionRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      direction_rotation_ = rotation_;
    }
    direction_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
  }
}

void Railgun::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
    if (charging_time_) {
      charging_time_--;
      for (int i = 0; i < 20; i++) {
        game_core_->PushEventGenerateParticle<particle::Smoke>(
            LocalToWorld(game_core_->RandomOnCircle() * 2.0f), rotation_,
            glm::vec2{0.0f}, 0.1f, glm::vec4{0.541f, 0.784f, 0.8745f, 1.0f},
            3.0f);
        game_core_->PushEventGenerateParticle<particle::Smoke>(
            LocalToWorld(game_core_->RandomOnCircle() * 2.0f), rotation_,
            glm::vec2{0.0f}, 0.1f, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 3.0f);
      }
    } else {
      if (should_shoot_) {
        auto velocity = Rotate(glm::vec2{0.0f, 40.0f}, direction_rotation_);
        GenerateBullet<bullet::Coin>(
            position_ + Rotate({0.0f, 1.2f}, direction_rotation_),
            direction_rotation_, GetDamageScale(), velocity, 20);
        should_shoot_ = false;
      }
    }
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        should_shoot_ = true;
        charging_time_ = 3 * kTickPerSecond;
        fire_count_down_ = 5 * kTickPerSecond;
      }
    }
  }
}

void Railgun::Thunderbolt() {
  skills_[0].time_remain = thunderbolt_count_down_;
  if (thunderbolt_count_down_) {
    thunderbolt_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        auto &units = game_core_->GetUnits();
        std::vector<size_t> id_list;
        for (auto &unit : units) {
          if (unit.first == id_) {
            continue;
          }
          id_list.push_back(unit.first);
        }
        size_t n = id_list.size();
        if (n == 1) {
          auto position = units.at(id_list[0])->GetPosition();
          game_core_->PushEventDealDamage(id_list[0], id_, 30.0f);
          game_core_->PushEventGenerateParticle<particle::Thunderbolt>(
              position, 0.0f, kTickPerSecond);
          thunderbolt_count_down_ = 15 * kTickPerSecond;
        }
        if (n > 1) {
          int i = game_core_->RandomInt(0, n - 1);
          auto position = units.at(id_list[1])->GetPosition();
          game_core_->PushEventDealDamage(id_list[1], id_, 30.0f);
          game_core_->PushEventGenerateParticle<particle::Thunderbolt>(
              position, 0.0f, kTickPerSecond);
          thunderbolt_count_down_ = 15 * kTickPerSecond;
        }
      }
    }
  }
}

bool Railgun::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *Railgun::UnitName() const {
  return "「Railgun」Misaka Mikoto";
}

const char *Railgun::Author() const {
  return "Aoarashi";
}
}  // namespace battle_game::unit
