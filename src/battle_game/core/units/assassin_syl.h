#pragma once
#include "battle_game/core/unit.h"
#include "battle_game/core/units/tiny_tank.h"

namespace battle_game::unit {
class AssassinSyl : public Tank {
 public:
  AssassinSyl(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void Fire();
  void InvincibleClick();
  void Invincible();
  void ChibakuTenseiClick();
  void ChibakuTensei();
  [[nodiscard]] float GetDamageScale();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  uint32_t fire_count_down_{0};
  uint32_t Invincible_count_down_{100};
  uint32_t ChibakuTensei_count_down_{1000};
  uint32_t is_invincible_{0};
  bool is_chibaku_tensei_{false};
};
}  // namespace battle_game::unit
