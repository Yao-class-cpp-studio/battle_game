#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "super_minesweeper_.h"

namespace battle_game::unit {
namespace {
uint32_t body_model_index = 0xffffffffu;
uint32_t turret_model_index = 0xffffffffu;
uint32_t frontal_model_index = 0xffffffffu;
uint32_t frontal_model_index_2 = 0xffffffffu;
uint32_t fore = 0xffffffffu;
uint32_t teleport = 0xffffffffu;
}  // namespace

Super_minesweeper::Super_minesweeper(GameCore *game_core,
                                     uint32_t id,
                                     uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Item Body */
      body_model_index = mgr->RegisterModel(
          {{{-0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3});
    }
    {
      /* Item Front */
      frontal_model_index = mgr->RegisterModel(
          {{{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
           {{-0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
           {{-0.5f, 0.8f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
           {{-0.5f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3});
    }
    {
      /* Item Front */
      frontal_model_index_2 = mgr->RegisterModel(
          {{{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
           {{0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
           {{0.5f, 0.8f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
           {{0.5f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3});
    }
    {
      /* Item Front */
      fore = mgr->RegisterModel(
          {{{-0.8f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
           {{0.8f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
           {{-0.6f, 1.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
           {{0.6f, 1.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3});
    }
    {
      /* Item Turret */
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
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});
      turret_vertices.push_back(
          {{0.75 * cos(glm::radians(120.0f)), 0.75 * sin(glm::radians(120.0f))},
           {0.0f, 0.0f},
           {0.0f, 0.0f, 1.0f, 1.0f}});
      turret_vertices.push_back(
          {{0.75 * cos(glm::radians(240.0f)), 0.75 * sin(glm::radians(240.0f))},
           {0.0f, 0.0f},
           {0.0f, 0.0f, 1.0f, 1.0f}});
      turret_vertices.push_back(
          {{0.75f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      turret_model_index = mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
  std::string a("penetrate");
  Skill penetrate(a, Q, 0, 600, 1, 1);
  penetrate.description = "Shoots through all targets, excluding obstacles";
  skills_.push_back(penetrate);
  std::string b("tracking");
  Skill tracking(b, R, 0, 1200, 1, 1);
  tracking.description = "Guided by the mouse cursor with a 10-second lifespan";
  skills_.push_back(tracking);
  std::string c("airborne_attack");
  Skill airborne_attack(c, E, 0, 1800, 1, 1);
  airborne_attack.description = "Bombard the target";
  airborne_attack.prompt =
      "Choose your target with direction keys and then press space to "
      "launch.\nPress E again to cancel.";
  skills_.push_back(airborne_attack);
  std::string d("bullets");
  Skill bullets(d, C, 0, 600, 1, 1);
  bullets.description = "In all directions and 3x";
  skills_.push_back(bullets);
  std::string e("teleportation");
  Skill teleportation(e, T, 0, 600, 1, 1);
  teleportation.description = "Go wherever you want";
  teleportation.prompt =
      "Choose your position by clicking.\nPress T again to cancel.";
  skills_.push_back(teleportation);
}
void Super_minesweeper::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(body_model_index);
  battle_game::DrawModel(frontal_model_index);
  battle_game::DrawModel(frontal_model_index_2);
  battle_game::DrawModel(fore);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(turret_model_index);
}
void Super_minesweeper::Update() {
  ItemMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}
void Super_minesweeper::ItemMove(float move_speed, float rotate_angular_speed) {
  if (teleport_timer_) {
    teleport_timer_--;
    skills_[4].time_remain = teleport_timer_;
  }
  if (teleport_buffer) {
    teleport_buffer--;
  }
  if (airborne_timer) {
    airborne_timer--;
    skills_[2].time_remain = airborne_timer;
  }
  if (airborne_buffer) {
    airborne_buffer--;
  }
  if (airborne_control_buffer) {
    airborne_control_buffer--;
  }
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
    if (input_data.key_down[GLFW_KEY_T] && !teleport_buffer) {
      if (!teleport_timer_ && !is_teleport) {
        is_teleport = true;
        skills_[4].is_prompt = true;
      } else if (is_teleport) {
        is_teleport = false;
        skills_[4].is_prompt = false;
      }
      teleport_buffer = 20;
    }
    if (input_data.key_down[GLFW_KEY_E] && !airborne_buffer) {
      if (!airborne_timer && !is_airborne) {
        is_airborne = true;
        skills_[2].is_prompt = true;
        airborne_target = position_;
      } else if (is_airborne) {
        is_airborne = false;
        skills_[2].is_prompt = false;
      }
      airborne_buffer = 20;
    }
    if (is_airborne) {
      if (timer_ % 30 == 0) {
        int i = 0;
        while (i < 120) {
          auto velocity =
              Rotate(glm::vec2{0.0f, 5.0f}, glm::radians(float(3 * i)));
          game_core_->PushEventGenerateParticle<particle::Signal>(
              airborne_target, rotation_, velocity, 0.2f,
              glm::vec4{1.0f, 0.0f, 0.0f, 0.03f}, 6.0f);
          i++;
        }
      }
      int i = 0;
      while (i < 4) {
        float factor = 1;
        if (timer_ % 60 < 15) {
          factor = 1.0 - float(timer_ % 60) / 60;
        } else if (timer_ % 60 < 30) {
          factor = 0.5 + float(timer_ % 60) / 60;
        }
        game_core_->PushEventGenerateParticle<particle::Penetrating>(
            airborne_target +
                glm::vec2{0.9 * factor * cos(glm::radians(float(90 * i))),
                          0.9 * factor * sin(glm::radians(float(90 * i)))},
            glm::radians(float(90.0 + i * 90)), glm::vec2{0.0f, 0.0f}, 0.6f,
            glm::vec4{1.0f, 1.0f, 0.0f, 1.0f}, 3.0f);
        i++;
      }
    }
    if (input_data.key_down[GLFW_KEY_SPACE]) {
      if (is_airborne) {
        landing = 60;
        is_airborne = false;
        airborne_timer = 1800;
        skills_[2].time_remain = 1800;
        skills_[2].is_prompt = false;
      }
    }
    if (landing) {
      game_core_->PushEventGenerateParticle<particle::Bullet_storage>(
          airborne_target, rotation_, glm::vec2{0.0f, 0.0f},
          float(landing) / 120,
          glm::vec4{0.0f, 0.0f, 0.0f, float(landing) / 120 + 0.5}, 3.0f);
      if (landing == 1) {
        int j = 0;
        while (j < 60) {
          auto velocity =
              Rotate(glm::vec2{0.0f, 10.0f}, glm::radians(float(6 * j)));
          GenerateBullet<bullet::Airborne_bomb>(airborne_target,
                                                glm::radians(float(6 * j)),
                                                GetDamageScale(), velocity);
          j++;
        }
        game_core_->PushEventGenerateParticle<particle::Expansion>(
            airborne_target, rotation_, glm::vec2{0.0f, 0.0f}, 0.0,
            glm::vec4{1.0f, 0.5f, 0.0f, 0.0f}, 3.0f);
      }
      landing--;
    }
    if (is_airborne && !airborne_control_buffer) {
      auto player = game_core_->GetPlayer(player_id_);
      if (player) {
        auto &input_data = player->GetInputData();
        if (input_data.key_down[GLFW_KEY_UP]) {
          airborne_target += glm::vec2{0.0f, 1.0f};
          if (game_core_->IsBlockedByObstacles(airborne_target)) {
            airborne_target -= glm::vec2{0.0f, 1.0f};
          }
          airborne_control_buffer = 20;
        }
        if (input_data.key_down[GLFW_KEY_DOWN]) {
          airborne_target += glm::vec2{0.0f, -1.0f};
          if (game_core_->IsBlockedByObstacles(airborne_target)) {
            airborne_target -= glm::vec2{0.0f, -1.0f};
          }
          airborne_control_buffer = 20;
        }
        if (input_data.key_down[GLFW_KEY_LEFT]) {
          airborne_target += glm::vec2{-1.0f, 0.0f};
          if (game_core_->IsBlockedByObstacles(airborne_target)) {
            airborne_target -= glm::vec2{-1.0f, 0.0f};
          }
          airborne_control_buffer = 20;
        }
        if (input_data.key_down[GLFW_KEY_RIGHT]) {
          airborne_target += glm::vec2{1.0f, 0.0f};
          if (game_core_->IsBlockedByObstacles(airborne_target)) {
            airborne_target -= glm::vec2{1.0f, 0.0f};
          }
          airborne_control_buffer = 20;
        }
      }
    }
    if (is_teleport) {
      if (player) {
        auto &input_data = player->GetInputData();
        auto position = input_data.mouse_cursor_position;
        float i = 1;
        while (i < 100) {
          float angle = glm::radians(float(10 * timer_ - i));
          glm::vec2 position1 =
              position + glm::vec2{0.7 * cos(angle), 0.7 * sin(angle)};
          game_core_->PushEventGenerateParticle<particle::Ring_segments>(
              position1, rotation_, glm::vec2{0.0f, 0.0f}, 0.12f,
              glm::vec4{0.0f, 0.0f, 1.0f, 0.1f}, 3.0f);
          i *= 1.35;
        }
        i = 1;
        while (i < 100) {
          float angle = glm::radians(float(10 * timer_ - i + 180));
          glm::vec2 position1 =
              position + glm::vec2{0.7 * cos(angle), 0.7 * sin(angle)};
          game_core_->PushEventGenerateParticle<particle::Ring_segments>(
              position1, rotation_, glm::vec2{0.0f, 0.0f}, 0.12f,
              glm::vec4{0.0f, 0.0f, 1.0f, 0.1f}, 3.0f);
          i *= 1.35;
        }
      }
    }
    if (!teleport_timer_) {
      float i = 1, j = 0;
      while (j < 6) {
        i = 1;
        while (i < 30) {
          float angle = turret_rotation_ + glm::radians(75.0 - i + 60 * j);
          glm::vec2 position1 =
              position_ + glm::vec2{2.0 * cos(angle), 2.0 * sin(angle)};
          game_core_->PushEventGenerateParticle<particle::Ring_segments>(
              position1, rotation_, glm::vec2{0.0f, 0.0f}, 0.15f,
              glm::vec4{1.0f, 0.0f, 1.0f, 0.1f}, 3.0f);
          i *= 1.35;
        }
        j++;
      }
    }
    if (!timer_penetrate_) {
      auto vector = glm::vec2{-0.64, -0.8};
      auto angle = rotation_ + std::atan2(5.0f, 4.0f) + glm::radians(180.0);
      auto position = position_ + glm::vec2{length(vector) * cos(angle),
                                            length(vector) * sin(angle)};
      game_core_->PushEventGenerateParticle<particle::Penetrating>(
          position, rotation_, glm::vec2{0.0f, 0.0f}, 0.65f,
          glm::vec4{1.0f, 1.0f, 0.0f, 1.0f}, 3.0f);
    }
    if (!tracking_timer_) {
      auto vector = glm::vec2{-0.64, -0.8};
      auto angle = rotation_ - std::atan2(5.0f, 4.0f);
      auto position = position_ + glm::vec2{length(vector) * cos(angle),
                                            length(vector) * sin(angle)};
      game_core_->PushEventGenerateParticle<particle::Tracking>(
          position, rotation_, glm::vec2{0.0f, 0.0f}, 0.55f,
          glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
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

void Super_minesweeper::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    timer_++;
    if (timer2_) {
      timer2_--;
    }
    turret_rotation_ = glm::radians(float(timer_));
    if (timer_ >= 360) {
      timer_ %= 360;
    }
  }
}

void Super_minesweeper::Fire() {
  if (is_second && !timer2_) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, 0.0f);
      Fireout(velocity);
      is_second = false;
      is_third = true;
      timer2_ = 20;
    }
  }
  if (is_third && !timer2_) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, 0.0f);
      Fireout(velocity);
      is_third = false;
    }
  }
  if (buffer_timer_) {
    buffer_timer_--;
  }
  if (fire_count_down_) {
    fire_count_down_--;
    skills_[3].time_remain = fire_count_down_;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        if (!buffer_timer_ && !is_teleport) {
          auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, 0.0f);
          Fireout(velocity);
          is_second = true;
          timer2_ = 20;
          fire_count_down_ = 10 * kTickPerSecond;  // Fire interval 10 seconds.
          skills_[3].time_remain = fire_count_down_;
        }
      }
    }
  }
  if (fire_count_down_ > 0 && fire_count_down_ < 600) {
    auto time = floor((599 - fire_count_down_) / 50);
    int i = 0;
    while (i < time) {
      float angle = -turret_rotation_ + i * glm::radians(30.0f);
      glm::vec2 position =
          position_ + glm::vec2{1.2 * cos(angle), 1.2 * sin(angle)};
      game_core_->PushEventGenerateParticle<particle::Bullet_storage>(
          position, rotation_, glm::vec2{0.0f, 0.0f}, 0.15f,
          glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 3.0f);
      i++;
    }
  } else if (fire_count_down_ == 0) {
    int i = 0;
    while (i < 12) {
      float angle = -turret_rotation_ + i * glm::radians(30.0f);
      glm::vec2 position =
          position_ + glm::vec2{1.2 * cos(angle), 1.2 * sin(angle)};
      game_core_->PushEventGenerateParticle<particle::Bullet_storage>(
          position, rotation_, glm::vec2{0.0f, 0.0f}, 0.15f,
          glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 3.0f);
      i++;
    }
  }
  if (is_teleport) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto position = input_data.mouse_cursor_position;
        if (!game_core_->IsBlockedByObstacles(position)) {
          game_core_->PushEventMoveUnit(id_, position);
          is_teleport = false;
          skills_[4].is_prompt = false;
          teleport_timer_ = 600;
          skills_[4].time_remain = 600;
          buffer_timer_ = 20;
        }
      }
    }
  }
  if (timer_penetrate_) {
    timer_penetrate_--;
    skills_[0].time_remain = timer_penetrate_;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        auto velocity = Rotate(glm::vec2{0.0f, 12.0f}, rotation_);
        GenerateBullet<bullet::Penetrate>(
            position_ + Rotate({0.0f, 1.2f}, rotation_), rotation_,
            GetDamageScale(), velocity);
        timer_penetrate_ = 10 * kTickPerSecond;  // Fire interval 10 seconds.
        skills_[0].time_remain = 600;
      }
    }
  }
  if (tracking_timer_) {
    tracking_timer_--;
    skills_[1].time_remain = tracking_timer_;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_R]) {
        auto velocity = Rotate(glm::vec2{0.0f, 8.0f}, rotation_);
        GenerateBullet<bullet::Tracking>(
            position_ + Rotate({0.0f, 1.2f}, rotation_), rotation_,
            GetDamageScale(), velocity);
        tracking_timer_ = 20 * kTickPerSecond;  // Fire interval 20 seconds.
        skills_[1].time_remain = tracking_timer_;
      }
    }
  }
}

void Super_minesweeper::Fireout(glm::vec2 velocity) {
  int j = 0;
  while (j < 60) {
    velocity = Rotate(glm::vec2{0.0f, 20.0f}, glm::radians(float(6 * j)));
    GenerateBullet<bullet::CannonBall>(
        position_ + Rotate({0.0f, 1.2f}, glm::radians(float(6 * j))),
        glm::radians(float(6 * j)), GetDamageScale(), velocity);
    j++;
  }
}

bool Super_minesweeper::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -1.0f && position.x < 1.0f && position.y > -1.0f &&
         position.y < 1.0f;
}

const char *Super_minesweeper::UnitName() const {
  return "Super \"minesweeper\"";
}

const char *Super_minesweeper::Author() const {
  return "C161324";
}
}  // namespace battle_game::unit
