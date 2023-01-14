#include "battle_game/core/bullets/disball.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

#include<iostream>
#include<time.h>
#include<windows.h>

namespace battle_game::bullet {

DisBall::DisBall(GameCore *core,
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

void DisBall::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture("../../textures/particle3.png");
  DrawModel(0);
}

int DisBall::RandomInt(int x,int y) {
    if(x==y) return x;
    int a=114514;
    if(x-y>=0)  a =x-y;
    else  a =y-x;
    srand(time(0));
    return rand() % a;
}

void DisBall::Update() {
  position_ += velocity_ * kSecondPerTick;
  damage_scale_*=0.98;
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
      game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
     // game_core_->PushEventDealDamage(unit.first, id_, damage_scale_*(RandomInt(1 , 2)) * 10.0f);
      should_die = true;
    }
  }
  //1

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

DisBall::~DisBall() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
