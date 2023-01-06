#pragma once
#include "cstdint"
#include "input_data.h"

namespace battle_game {
class GameCore;
class Player {
 public:
  Player(GameCore *game_core, uint32_t id);
  [[nodiscard]] uint32_t GetId() const {
    return id_;
  }
  void SetInputData(const InputData &input_data) {
    input_data_ = input_data;
  }
  [[nodiscard]] const InputData &GetInputData() const {
    return input_data_;
  }
  [[nodiscard]] uint32_t GetPrimaryUnitId() const {
    return primary_unit_id_;
  }
  int &SelectedUnit() {
    return selected_unit_;
  }

  virtual void Update();
  [[nodiscard]] uint32_t GetResurrectionCountDown() const {
    return resurrection_count_down_;
  }

 protected:
  GameCore *game_core_{};
  uint32_t id_{};
  InputData input_data_{};
  uint32_t primary_unit_id_{};
  uint32_t resurrection_count_down_{1};
  int selected_unit_{0};
};

class AiPlayer : public Player {
 public:
  AiPlayer(GameCore *game_core, uint32_t id) : Player(game_core, id) {
  }
  void Update();

 protected:
  uint32_t fire_count_{30};
};
}  // namespace battle_game