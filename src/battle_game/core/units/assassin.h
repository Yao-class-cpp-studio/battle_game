#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Assassin : public Unit {
 public:
  Assassin(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void RenderHelper() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void AssassinMove(float move_speed);
  uint32_t InvisibleCoolDown() const;
  void InvisibleClick();
  void Invisible();
  uint32_t TeleportingCoolDown() const;
  void TeleportingClick();
  void Teleporting();
  void Click();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float knife_rotation_{0.0f};
  uint32_t attack_count_down_{0};
  uint32_t invisible_count_down_{0};
  uint32_t invisible_{0};
  uint32_t teleporting_count_down_{0};
  bool teleporting_{false};
};
}  // namespace battle_game::unit