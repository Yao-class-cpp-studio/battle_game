#include "lightening.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t Lightening_body_model_index = 0xffffffffu;
uint32_t Lightening_turret_model_index = 0xffffffffu;
}  // namespace

Lightening::Lightening(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Tank(game_core, id, player_id) {
  if (!~Lightening_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Lightening Body */
      Lightening_body_model_index = mgr->RegisterModel(
          {
              {{-0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              // stripes
              {{-0.8f, 0.83f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.8f, 0.70f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.4f, 0.83f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.4f, 0.70f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              {{0.8f, 0.83f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.8f, 0.70f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.4f, 0.83f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.4f, 0.70f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              {{0.8f, 0.57f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.8f, 0.44f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.4f, 0.57f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.4f, 0.44f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              {{-0.8f, 0.57f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.8f, 0.44f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.4f, 0.57f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.4f, 0.44f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              // back stripes
              {{-0.75f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.75f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.65f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.65f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              {{-0.57f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.57f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.47f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{-0.47f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              {{0.75f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.75f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.65f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.65f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              {{0.57f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.57f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.47f, -0.65f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
              {{0.47f, -1.00f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},

              {{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              // distinguish front and back
              {{0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},

          },
          //{0, 1, 2, 1, 2, 3, 0, 2, 5, 2, 4, 5});
          {0,  1,  3,  1,  3,  2,  4,  5,  6,  5,  6,  7,  8,  9,
           10, 9,  10, 11, 12, 13, 14, 13, 14, 15, 16, 17, 18, 17,
           18, 19, 20, 21, 22, 21, 22, 23, 24, 25, 26, 25, 26, 27,
           28, 29, 30, 29, 30, 31, 32, 33, 34, 33, 34, 35});
    }

    {
      /* Lightening Turret */
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
      Lightening_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }

  Skill temp;
  temp.description = "Shoot Three Rocket";
  temp.name = "Rocket";
  temp.time_remain = 0;
  temp.time_total = 300;
  temp.type = E;
  temp.function = SKILL_ADD_FUNCTION(Lightening::RocketClick);

  skills_.push_back(temp);

  temp.name = "Flash";
  temp.description = "gain a tremendous speed for a short period of time";
  temp.time_remain = 0;
  temp.time_total = 240;
  temp.type = SPACE;
  temp.function = SKILL_ADD_FUNCTION(Lightening::FlashClick);
  skills_.push_back(temp);
}

void Lightening::RocketClick() {
  rocket_count_down_ = 5 * kTickPerSecond;
  auto player = game_core_->GetPlayer(player_id_);
  if (game_core_->GetUnits().size() > 1) {
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    GenerateBullet<bullet::Rocket>(
        position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
        GetDamageScale(), velocity);
  }
}

void Lightening::Rocket() {
  skills_[0].time_remain = rocket_count_down_;
  if (rocket_count_down_) {
    rocket_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_E]) {
        RocketClick();
      }
    }
  }
  if (rocket_count_down_ == 5 * kTickPerSecond - kTickPerSecond / 5) {
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    GenerateBullet<bullet::Rocket>(
        position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
        GetDamageScale(), velocity);
  }
  if (rocket_count_down_ == 5 * kTickPerSecond - 2 * kTickPerSecond / 5) {
    auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
    GenerateBullet<bullet::Rocket>(
        position_ + Rotate({0.0f, 1.2f}, turret_rotation_), turret_rotation_,
        GetDamageScale(), velocity);
  }
}

void Lightening::FlashClick() {
  flash_count_down_ = 4 * kTickPerSecond;
  flash_duration_ = 1 * kTickPerSecond;
}

bool Lightening::Flash() {
  skills_[1].time_remain = flash_count_down_;
  if (flash_count_down_) {
    flash_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_SPACE]) {
        FlashClick();
      }
    }
  }
  if (flash_duration_) {
    --flash_duration_;
    return 1;
  }
  return 0;
}

void Lightening::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(Lightening_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(Lightening_turret_model_index);
}

void Lightening::Update() {
  bool idx = Flash();
  if (idx) {
    Tank::TankMove(13.0f, glm::radians(180.0f));
  } else {
    Tank::TankMove(4.5f, glm::radians(180.0f));
  }
  Flash();
  Tank::TurretRotate();
  Fire();
  Rocket();
}

void Lightening::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 27.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        auto offset = glm::radians(5.0f);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + offset),
            turret_rotation_ + offset, GetDamageScale(), velocity);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_ - offset),
            turret_rotation_ - offset, GetDamageScale(), velocity);

        fire_count_down_ = kTickPerSecond / 4;  // Fire interval 1 second.
      }
    }
  }
}

bool Lightening::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *Lightening::UnitName() const {
  return "Lightening";
}

const char *Lightening::Author() const {
  return "jason-huang";
}
}  // namespace battle_game::unit
