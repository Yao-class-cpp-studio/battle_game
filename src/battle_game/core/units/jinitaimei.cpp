#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "jinitaimei.h"

namespace battle_game::unit {
Jinitaimei::Jinitaimei(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {

  Skill temp;
  temp.name = "TieShanKao";
  temp.description = "MORE basketball!!!";
  temp.time_remain = 0;
  temp.time_total = 0;
  temp.type = E;
  skills_.push_back(temp);
}

void Jinitaimei::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture("../../textures/ji.png");
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(0);
  battle_game::SetRotation(turret_rotation_);
}//texture

void Jinitaimei::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void Jinitaimei::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 2.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 2.0f;
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

void Jinitaimei::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } 
    //else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    //}
  }
}//mouse

void Jinitaimei::Tieshankao() {
  auto velocity = Rotate(glm::vec2{1.0f, 10.0f}, turret_rotation_);
  GenerateBullet<bullet::SmallBasketball>(
      position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
      GetDamageScale() * 2, velocity);
}


void Jinitaimei::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_);
        GenerateBullet<bullet::Basketball>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale()*2, velocity);
        fire_count_down_ = kTickPerSecond/2;  // Fire interval 0.5 second.
      }
      if (input_data.key_down[GLFW_KEY_E]) {
        for (int i=0;i<=2;i++) {
          Tieshankao();
        }
      }
    }
  }
}

bool Jinitaimei::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}



const char *Jinitaimei::UnitName() const {
  return "JiNiTaiMei";
}

const char *Jinitaimei::Author() const {
  return "JujuMan";
}
}  // namespace battle_game::unit
