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
                                  unit->Author());                           \
  selectable_unit_list_skill_.push_back(true);

#define ADD_SELECTABLE_UNIT_WITHOUT_SKILL(UnitType)                          \
  unit = std::make_unique<UnitType>(nullptr, 0, 0);                          \
  AddPrimaryUnitAllocationFunction<UnitType>();                              \
  selectable_unit_list_.push_back(unit->UnitName() + std::string(" - By ") + \
                                  unit->Author());                           \
  selectable_unit_list_skill_.push_back(false);

  /*
   * TODO: Add Your Unit Here!
   * */
  ADD_SELECTABLE_UNIT(unit::InfernoTank);
  ADD_SELECTABLE_UNIT(unit::Tank);
  ADD_SELECTABLE_UNIT(unit::DoubleScatterTank);
  ADD_SELECTABLE_UNIT(unit::ThreeBodyMan);
  ADD_SELECTABLE_UNIT(unit::LMTank);
  ADD_SELECTABLE_UNIT(unit::MissileTank);
  ADD_SELECTABLE_UNIT(unit::TripleShotTank);
  ADD_SELECTABLE_UNIT(unit::TankXxy);
  ADD_SELECTABLE_UNIT(unit::WhaoooooTank);
  ADD_SELECTABLE_UNIT(unit::RoundUFO);
  ADD_SELECTABLE_UNIT(unit::DarkFury);
  ADD_SELECTABLE_UNIT(unit::MineSampleTank);
  ADD_SELECTABLE_UNIT(unit::SquareTank);
  ADD_SELECTABLE_UNIT(unit::ZibengDog);
  ADD_SELECTABLE_UNIT(unit::TankK);
  ADD_SELECTABLE_UNIT(unit::ReboundingSampleTank);
  ADD_SELECTABLE_UNIT(unit::RageTank);
  ADD_SELECTABLE_UNIT(unit::SmokeBombTank);
  ADD_SELECTABLE_UNIT(unit::Sparky);
  ADD_SELECTABLE_UNIT(unit::CritTank);
  ADD_SELECTABLE_UNIT(unit::Railgun);
  ADD_SELECTABLE_UNIT(unit::Udongein);

  unit.reset();
}
}  // namespace battle_game
