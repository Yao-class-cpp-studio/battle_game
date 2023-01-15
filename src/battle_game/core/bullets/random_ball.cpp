#include "battle_game/core/bullets/random_ball.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {

namespace {
std::vector<std::function<
    void(battle_game::Unit *, glm::vec2, float, float, glm::vec2)>>
    crit_bullet_type_list;
bool is_crit_bullet_type_set = false;
#define ADD_SELECTABLE_CRIT_BULLET(BulletType)                             \
  crit_bullet_type_list.push_back([](battle_game::Unit *my_unit,           \
                                     glm::vec2 position, float rotation,   \
                                     float damage_scale, glm::vec2 args) { \
    my_unit->GenerateBullet<bullet::BulletType>(position, rotation,        \
                                                damage_scale, args);       \
  });
void set_crit_bullet_type() {
  // Welcome to add your bullet here!
  // Note: Only \ BulletType / having
  // \ GenerateBullet<bullet::BulletType>(glm::vec2, float, float, glm::vec2) /
  // will be accepted.
  ADD_SELECTABLE_CRIT_BULLET(CannonBall);
  // ADD_SELECTABLE_CRIT_BULLET(Mine); // It is AWFUL!
  ADD_SELECTABLE_CRIT_BULLET(ReboundingBall);
  ADD_SELECTABLE_CRIT_BULLET(Rocket);
  ADD_SELECTABLE_CRIT_BULLET(SweatySoybean);
  ADD_SELECTABLE_CRIT_BULLET(WaterDrop);
  //      ADD_SELECTABLE_CRIT_BULLET(CovidBall);
  ADD_SELECTABLE_CRIT_BULLET(RandomBall);  // Matryoshka dolls, happy!
}
}  // namespace

RandomBall::RandomBall(GameCore *core,
                       uint32_t id,
                       uint32_t unit_id,
                       uint32_t player_id,
                       glm::vec2 position,
                       float rotation,
                       float damage_scale,
                       glm::vec2 velocity)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity) {
}

void RandomBall::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/random_ball.png");
  DrawModel(0);
}

void RandomBall::Update() {
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == player_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      if (game_core_->RandomFloat() <= crit_rate_) {
        if (!is_crit_bullet_type_set) {
          set_crit_bullet_type();
          is_crit_bullet_type_set = true;
        }
        auto my_unit = game_core_->GetUnit(unit_id_);
        float now_theta = 0.0f;            // angle to i
        glm::vec2 now_dir = {0.0f, 0.0f};  // unit vector to now_theta
        glm::vec2 now_pos = {0.0f, 0.0f};  // position to i
        float now_rot = 0.0f;              // rotation to i
        glm::vec2 now_v = {0.0f, 0.0f};    // velocity to i
        for (int i = 0; i < crit_number_; i++) {
          now_theta = 2.0f * glm::pi<float>() * float(i) / crit_number_;
          now_dir = glm::vec2{std::cos(now_theta), std::sin(now_theta)};
          now_pos = unit.second->GetPosition() + crit_radius_ * now_dir;
          now_rot = glm::pi<float>() + now_theta;
          now_v = -std::powf(glm::dot(velocity_, velocity_), 0.5f) * now_dir;
          crit_bullet_type_list[game_core_->RandomInt(
              0, crit_bullet_type_list.size() - 1)](
              my_unit, now_pos, now_rot, damage_scale_ / float(crit_number_),
              now_v);
        }
      }
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

RandomBall::~RandomBall() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
