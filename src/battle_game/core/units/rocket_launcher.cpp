#include "rocket_launcher.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

/*这是一个火箭车，较宽一端为车头
-开火：每四秒可以发射一枚火箭，直接瞄准方框内任意一点（无视障碍）
-开火后先出现瞄准标记，2秒后火箭打击目标点附近，打击后1秒内可以造成伤害
-打击后出现短暂烟雾，若击中敌方目标，则出现长期烟雾
-距离目标越远，精度越低，可能无法击中目标
-火箭伤害为正常伤害的3倍
-火箭车会受到后坐力影响，开火后1秒内向开火方向反方向移动，且该秒内不受控制
*/

namespace battle_game::unit {
namespace {
uint32_t cart_body_model_index = 0xffffffffu;
uint32_t cart_turret_model_index = 0xffffffffu;
}  // namespace
Cart::Cart(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  if (!~cart_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Cart Body */
      cart_body_model_index = mgr->RegisterModel(
          {{{-0.55f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.55f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.55f, 0.6f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.55f, 0.6f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.5f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.5f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7});
    }
    {
      /* Cart Turret */
      cart_turret_model_index = mgr->RegisterModel(
          {{{-0.2f, 0.9f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}},
           {{0.2f, 0.9f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}},
           {{-0.2f, -0.9f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}},
           {{0.2f, -0.9f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}},
           {{0.0f, 1.1f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}}},
          {0, 1, 2, 1, 2, 3, 0, 1, 4});
    }
  }
}
void Cart::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(cart_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(cart_turret_model_index);
}
void Cart::Update() {
  CartMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}
void Cart::CartMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (fire_count_down_ >= 3 * kTickPerSecond)
      offset.y -= 0.3f;
    else {
      if (input_data.key_down[GLFW_KEY_W]) {
        offset.y += 1.0f;
      }
      if (input_data.key_down[GLFW_KEY_S]) {
        offset.y -= 1.0f;
      }
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    glm::vec2 new_position;
    if (fire_count_down_ >= 3 * kTickPerSecond)
      new_position =
          position_ +
          glm::vec2{glm::rotate(glm::mat4{1.0f}, backforce_direction_,
                                glm::vec3{0.0f, 0.0f, 1.0f}) *
                    glm::vec4{offset, 0.0f, 0.0f}};
    else
      new_position =
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

void Cart::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
    if (fire_count_down_ == 2 * kTickPerSecond) {
      GenerateBullet<bullet::AirRocket>(
          target_ + game_core_->RandomInCircle() * 0.15f * distance_, 0.0f,
          3 * GetDamageScale());
    }
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        target_ = input_data.mouse_cursor_position;
        glm::vec2 diff = input_data.mouse_cursor_position - position_;
        distance_ = glm::length(diff);
        backforce_direction_ = turret_rotation_;
        game_core_->PushEventGenerateParticle<particle::Target>(
            target_, 0.0f, game_core_->GetPlayerColor(player_id_));
        fire_count_down_ = 4 * kTickPerSecond;
      }
    }
  }
}
bool Cart::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.5f && position.x < 0.5f && position.y > -1.0f &&
         position.y < 1.0f;
}
const char *Cart::UnitName() const {
  return "Rocket Launcher";
}

const char *Cart::Author() const {
  return "Photon Zheng";
}

}  // namespace battle_game::unit
