// Copyright (c) 2019 Sarun Rattanasiri
// under the 3-Clause BSD License
// https://opensource.org/licenses/BSD-3-Clause

#include "haystack_command_encoder.h"

void haystack_command_encoder__poll(HaystackCommandEncoder_Config* config,
                                    HaystackCommandEncoder_State* state) {
  if (!(state->processing_command)) {
    // select command
    uint8_t index_scanner = state->index_scanner;
    if (HAYSTACK_COMMAND_ENCODER__BUFFER_SIZE <=
        index_scanner) {  // index_scanner overflown
      state->index_scanner = 0;
      return;
    }
    uint8_t current_buffer = state->buffer[index_scanner];
    uint8_t bit_scanner = state->bit_scanner;
    if (!current_buffer ||  // current_buffer has no bit set
        !bit_scanner) {     // bit_scanner overflown
      // move the scanner
      state->index_scanner = index_scanner + 1;
      state->bit_scanner = 1;
      state->decoded_bit = 0;
      return;
    }
    uint8_t decoded_bit = state->decoded_bit;
    if (current_buffer & bit_scanner) {
      state->buffer[index_scanner] = (current_buffer & ~bit_scanner);
      state->current_command = (index_scanner << 3) | decoded_bit;
      state->processing_command = true;
    }
    state->bit_scanner = bit_scanner << 1;
    state->decoded_bit = decoded_bit + 1;
    return;
  }
  // process command
  if (!config->port_ready)
    return;
  if (!config->port_ready())
    return;
  uint8_t next_key_position = state->next_key_position;
  if (next_key_position < HAYSTACK_COMMAND_ENCODER__KEY_SIZE) {
    state->next_key_position = next_key_position + 1;
    config->port_flush(config->key[next_key_position]);
    return;
  }
  state->next_key_position = 0;
  state->processing_command = false;
  config->port_flush(state->current_command);
}

void haystack_command_encoder__queue(HaystackCommandEncoder_State* state,
                                     uint8_t command) {
  if (HAYSTACK_COMMAND_ENCODER__COMMAND_CAPACITY <= command)
    return;
  state->buffer[command >> 3] |= (1 << (command % 8));
}
