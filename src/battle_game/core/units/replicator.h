#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Replicator : public Unit {
 public:
  Replicator(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  void IsHit_Modify();
  void Fire();
  ~Replicator();

 protected:
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  int ishit_count_;
  int type_id_;
  int obstacle_id_;
};
}  // namespace battle_game::unit
