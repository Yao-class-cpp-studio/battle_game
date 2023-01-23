#include "battle_game/app/message.h"

#include <string>

namespace battle_game {
MessageInitial::MessageInitial() : player_cnt(0u), player_id(0u) {
}
MessageInitial::MessageInitial(uint32_t player_cnt, uint32_t player_id)
    : player_cnt(player_cnt), player_id(player_id) {
}
MessageInitial::MessageInitial(const uint8_t *message)
    : player_cnt(message[0]), player_id(message[1]) {
}
MessageInitial::operator ByteString() const {
  ByteString result;
  result.resize(2);
  result[0] = player_cnt;
  result[1] = player_id;
  return result;
}

MessageInputData::MessageInputData() : selected_unit(0u), input_data{} {
  std::memset(input_data.key_down, 0, sizeof(input_data.key_down));
  std::memset(input_data.mouse_button_down, 0,
              sizeof(input_data.mouse_button_down));
  std::memset(input_data.mouse_button_clicked, 0,
              sizeof(input_data.mouse_button_clicked));
}
MessageInputData::MessageInputData(uint32_t selected_unit,
                                   const InputData &input_data)
    : selected_unit(selected_unit), input_data(input_data) {
}
MessageInputData::MessageInputData(const uint8_t *message)
    : selected_unit(message[0]) {
  size_t totbit = 8u;
  for (size_t i = 0; i < kKeyRange; ++i, ++totbit) {
    input_data.key_down[i] = (message[totbit >> 3] >> (totbit & 7)) & 1;
  }
  for (size_t i = 0; i < kMouseButtonRange; ++i, ++totbit) {
    input_data.mouse_button_down[i] =
        (message[totbit >> 3] >> (totbit & 7)) & 1;
  }
  for (size_t i = 0; i < kMouseButtonRange; ++i, ++totbit) {
    input_data.mouse_button_clicked[i] =
        (message[totbit >> 3] >> (totbit & 7)) & 1;
  }
  memcpy(&input_data.mouse_cursor_position, message + ((totbit - 1) >> 3) + 1,
         sizeof(glm::vec2));
}
MessageInputData::operator ByteString() const {
  uint8_t result[length]{};
  size_t totbit = 8u;
  memset(result, 0, sizeof(result));
  result[0] = selected_unit;
  for (size_t i = 0; i < kKeyRange; ++i, ++totbit) {
    result[totbit >> 3] |= (input_data.key_down[i] ? 1u : 0u) << (totbit & 7);
  }
  for (size_t i = 0; i < kMouseButtonRange; ++i, ++totbit) {
    result[totbit >> 3] |= (input_data.mouse_button_down[i] ? 1u : 0u)
                           << (totbit & 7);
  }
  for (size_t i = 0; i < kMouseButtonRange; ++i, ++totbit) {
    result[totbit >> 3] |= (input_data.mouse_button_clicked[i] ? 1u : 0u)
                           << (totbit & 7);
  }
  memcpy(result + ((totbit - 1) >> 3) + 1, &input_data.mouse_cursor_position,
         sizeof(glm::vec2));
  return ByteString(result, length);
}
}  // namespace battle_game
