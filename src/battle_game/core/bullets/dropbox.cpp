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
                 const std::string effect,
                 float scale = 1.5f)
    : Bullet(core, id, unit_id, player_id, position, rotation, 0.0f),
      effect_(effect),
      scale_(scale),
      lifetime_(10.0f) {
}
void DropBox::Update() {
  lifetime_ -= kSecondPerTick;
  strength_ = std::sin((lifetime_ + 1 / 2) * glm::pi<float>());
  auto &units = game_core_->GetUnits();
  if (is_got_ == 0) {
    for (auto &unit : units) {
      if (unit.second->IsHit(position_)) {
        if (effect_ == "heal") {
          game_core_->PushEventHeal(unit.first, 20.0f);
          game_core_->PushEventRemoveBullet(id_);
        } else {
          game_core_->PushEventSetEffect(unit.first, effect_, scale_);
          is_got_ = unit.first;
          lifetime_ = 20.0f;
          break;
        }
      }
    }
  }
  if (lifetime_ <= 0) {
    if (is_got_ != 0) {
      game_core_->PushEventSetEffect(is_got_, effect_, 1.0f);
    }
    game_core_->PushEventRemoveBullet(id_);
  }
}

void DropBox::Render() {
  if (is_got_ == 0) {
    SetTransformation(position_, rotation_, glm::vec2{0.5f});
    if (lifetime_ >= 3)
      SetColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
    else
      SetColor(glm::vec4{1.0f, 1.0f, 1.0f, strength_ * strength_});
    SetTexture(std::string("../../textures/" + effect_ + ".png"));
    DrawModel(0);
  }
}
}  // namespace battle_game::bullet
