#include "battle_game/core/game_core.h"

namespace battle_game {

template <class UnitType, class... Args>
void GameCore::AddPrimaryUnitAllocationFunction(Args... args) {
  primary_unit_allocation_functions_.emplace_back([=](uint32_t player_id) {
    return AddUnit<UnitType>(player_id, args...);
  });
}

void GameCore::GeneratePrimaryUnitList() {
  std::unique_ptr<Unit> unit;

#define ADD_SELECTABLE_UNIT(UnitType)                                        \
  unit = std::make_unique<UnitType>(nullptr, 0, 0);                          \
  AddPrimaryUnitAllocationFunction<UnitType>();                              \
  selectable_unit_list_.push_back(unit->UnitName() + std::string(" - By ") + \
                                  unit->Author());

  /*
   * TODO: Add Your Unit Here!
   * */
  ADD_SELECTABLE_UNIT(unit::Tank);
  ADD_SELECTABLE_UNIT(unit::DoubleScatterTank);
  ADD_SELECTABLE_UNIT(unit::ThreeBodyMan);
  ADD_SELECTABLE_UNIT(unit::InfernoTank);
  ADD_SELECTABLE_UNIT(unit::Triple_shot_star_tank);
  ADD_SELECTABLE_UNIT(unit::WhaoooooTank);

  unit.reset();
}
}  // namespace battle_game
