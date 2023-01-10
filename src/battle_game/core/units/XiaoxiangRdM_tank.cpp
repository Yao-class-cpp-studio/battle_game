#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

XXRdMTank::XXRdMTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  
  Skill tmp;
  tmp.name = "Teleport (Forward)";
  tmp.description = "Instantaneously Moving Forward While Shooting";
  tmp.time_remain = 360;
  tmp.time_total = 180;
  tmp.type = E;
  tmp.function = SKILL_ADD_FUNCTION(XXRdMTank::TeleportFClick);
  skills_.push_back(tmp);
  tmp.name = "Teleport (Backward)";
  tmp.description = "Instantaneously Moving Backward While Shooting";
  tmp.time_remain = 360;
  tmp.time_total = 180;
  tmp.type = Q;
  tmp.function = SKILL_ADD_FUNCTION(XXRdMTank::TeleportBClick);
  skills_.push_back(tmp);
  tmp.name = "Healthcare";
  tmp.description = "Improve Health Value";
  tmp.time_remain = 1200;
  tmp.time_total = 1200;
  tmp.type = R;
  tmp.function = SKILL_ADD_FUNCTION(XXRdMTank::HealthcareClick);
  skills_.push_back(tmp);
  

  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();

    {
      /* Tank Turret */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }

    {
      /* Tank Body */
      std::vector<ObjectVertex> body_vertices;
      std::vector<uint32_t> body_indices;
      const int precision = 600;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        auto x= sin_theta * x_0_, y = cos_theta * y_0_;
        if(y > 0 && abs(x) < .6 && abs(x) > .5) y = .1;
        body_vertices.push_back({{x, y},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        body_indices.push_back(i);
        body_indices.push_back((i + 1) % precision);
        body_indices.push_back(precision);
      }
      tank_body_model_index = 
          mgr->RegisterModel(body_vertices, body_indices);
    }
  }
}

void XXRdMTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void XXRdMTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Teleport();
  Fire();
  Healthcare();
}

void XXRdMTank::TankMove(float move_speed, float rotate_angular_speed) {
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

void XXRdMTank::TurretRotate() {
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

void XXRdMTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
      }
    }
  }
}

void XXRdMTank::TeleportClick(bool isforward){
  glm::vec2 offset{0.0f};
  auto new_position = position_;
  float x_1 = -.05*sin(rotation_), 
        y_1 = .05*cos(rotation_);
  if(!isforward) {
    x_1 = -x_1 , y_1 = - y_1;
  }
  // std::cout<<rotation_<<std::endl;
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
  for(uint16_t i = 0 ; i < 50 ; i++ ){
    new_position.x += x_1;
    new_position.y += y_1;
    if(game_core_->IsBlockedByObstacles(new_position)) {
      break;
    }
    if(!(i%7)){
      GenerateBullet<bullet::CannonBall>(
      new_position + Rotate({0.0f, 1.2f}, turret_rotation_),
      turret_rotation_, GetDamageScale(), velocity);
    }
    game_core_->PushEventMoveUnit(id_, new_position);
  }
  teleport_count_down_ = 3 * kTickPerSecond;  // 3 second.
}
void XXRdMTank::TeleportFClick(){
  TeleportClick(TRUE);
}
void XXRdMTank::TeleportBClick(){
  TeleportClick(FALSE);
}

void XXRdMTank::Teleport(){
  skills_[0].time_remain = skills_[1].time_remain = teleport_count_down_;
  if (teleport_count_down_) {
    teleport_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) TeleportFClick();
      else if(input_data.key_down[GLFW_KEY_Q]) TeleportBClick();
    }
  }
}

void XXRdMTank::HealthcareClick(){
  healthcare_count_down_ = 20 * kTickPerSecond;
  float new_health = health_ + .1;
  if(new_health > 1) new_health = 1;
  SetHealth(new_health);
}

void XXRdMTank::Healthcare(){
  skills_[2].time_remain = healthcare_count_down_;
  if(!teleport_count_down_){
    if (healthcare_count_down_) {
      healthcare_count_down_--;
    } else {
      auto player = game_core_->GetPlayer(player_id_);
      if (player) {
        auto &input_data = player->GetInputData();
        if (input_data.key_down[GLFW_KEY_R]) HealthcareClick();
      }
    }
  }
}

bool XXRdMTank::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return pow(position.x/x_0_ , 2) + pow(position.y/y_0_ , 2) <= 1;
}

const char *XXRdMTank::UnitName() const {
  return "Ellipse_Tank";
}

const char *XXRdMTank::Author() const {
  return "Chen Rui Qin Fang";
}
}  // namespace battle_game::unit
