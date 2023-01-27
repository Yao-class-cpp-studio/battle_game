#pragma once
#include <string>
#include <vector>

#include "battle_game/core/input_data.h"

namespace battle_game {
typedef std::basic_string<uint8_t> ByteString;

struct MessageInitial {
  MessageInitial();
  MessageInitial(uint32_t player_cnt, uint32_t player_id);
  MessageInitial(const uint8_t *message);
  operator ByteString() const;
  static constexpr size_t length = 2;
  uint32_t player_cnt, player_id;
};

struct MessageInputData {
  MessageInputData();
  MessageInputData(uint32_t selected_unit, const InputData &input_data);
  MessageInputData(const uint8_t *message);
  operator ByteString() const;
  static constexpr size_t length =
      1 + ((kKeyRange + kMouseButtonRange * 2 + 7) >> 3) + sizeof(glm::vec2);
  uint32_t selected_unit;
  InputData input_data;
};

typedef std::vector<MessageInputData> CompleteInputData;
}  // namespace battle_game
