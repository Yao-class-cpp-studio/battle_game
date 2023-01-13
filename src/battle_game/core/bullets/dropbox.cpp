#include "battle_game/core/bullets/dropbox.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"

namespace battle_game::bullet {
DropBox::DropBox(GameCore *core,
                 uint32_t id,
                 uint32_t unit_id,
                 uint32_t player_id,
                 glm::vec2 position,
                 float rotation,
                 float damage_scale,
                 uint32_t type,
                 float scale = 1.5f)
    : Bullet(core, id, unit_id, player_id, position, rotation, 0.0f),
      type_(type),
      scale_(scale),
      lifetime_(10.0f) {
  if (type >= 5)
    game_core_->PushEventRemoveBullet(id_);
}
void DropBox::Update() {
  lifetime_ -= kSecondPerTick;
  strength_ = std::sin((lifetime_ + 1 / 2) * glm::pi<float>());
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.second->IsHit(position_)) {
      if (type_ == 4) {
        game_core_->PushEventHeal(unit.first, 20.0f);
        game_core_->PushEventRemoveBullet(id_);
      } else if (type_ == 3) {
        game_core_->PushEventSetEffect(unit.first, Unit::BuffType(type_),
                                       scale_, 3.0f);
      } else {
        game_core_->PushEventSetEffect(unit.first, Unit::BuffType(type_),
                                       scale_, 20.0f);
        game_core_->PushEventRemoveBullet(id_);
      }
    }
  }
  if (lifetime_ <= 0) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

void DropBox::Render() {
  if (type_ == 3) {
    SetTransformation(position_, rotation_, glm::vec2{0.5f});
    SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 0.1 * lifetime_});
    SetTexture("../../textures/" + typenames_[type_] + ".png");
    DrawModel(0);
  } else {
    SetTransformation(position_, rotation_, glm::vec2{0.5f});
    if (lifetime_ >= 3)
      SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    else
      SetColor(glm::vec4{1.0f, 1.0f, 1.0f, strength_ * strength_});
    SetTexture("../../textures/" + typenames_[type_] + ".png");
    DrawModel(0);
  }
}
}  // namespace battle_game::bullet
