// Copyright (c) 2019 Sarun Rattanasiri
// under the 3-Clause BSD License
// https://opensource.org/licenses/BSD-3-Clause

#ifndef __HAYSTACK_COMMAND_ENCODER_H
#define __HAYSTACK_COMMAND_ENCODER_H

#ifndef HAYSTACK_COMMAND_ENCODER__KEY_SIZE
#define HAYSTACK_COMMAND_ENCODER__KEY_SIZE 16
#endif

#ifndef HAYSTACK_COMMAND_ENCODER__COMMAND_CAPACITY
#define HAYSTACK_COMMAND_ENCODER__COMMAND_CAPACITY 8
#endif

#define HAYSTACK_COMMAND_ENCODER__BUFFER_SIZE \
  (HAYSTACK_COMMAND_ENCODER__COMMAND_CAPACITY / 8)

#include <stdbool.h>
#include <stdint.h>

typedef struct _haystack_command_encoder__config {
  void (*port_flush)(uint8_t data);
  bool (*port_ready)(void);
  uint8_t key[HAYSTACK_COMMAND_ENCODER__KEY_SIZE];
} HaystackCommandEncoder_Config;

typedef struct _haystack_command_encoder__state {
  uint8_t bit_scanner;
  uint8_t current_command;
  uint8_t decoded_bit;
  uint8_t index_scanner;
  uint8_t next_key_position;
  bool processing_command;
  uint8_t buffer[HAYSTACK_COMMAND_ENCODER__BUFFER_SIZE];
} HaystackCommandEncoder_State;

#define haystack_command_encoder__init(state)                 \
  do {                                                        \
    (state)->bit_scanner = 1;                                 \
    (state)->decoded_bit = 0;                                 \
    (state)->index_scanner = 0;                               \
    (state)->next_key_position = 0;                           \
    (state)->processing_command = false;                      \
    for (uint8_t haystack_command_encoder__counter = 0;       \
         haystack_command_encoder__counter <                  \
         HAYSTACK_COMMAND_ENCODER__BUFFER_SIZE;               \
         haystack_command_encoder__counter++) {               \
      (state)->buffer[haystack_command_encoder__counter] = 0; \
    }                                                         \
  } while (0)

void haystack_command_encoder__queue(HaystackCommandEncoder_State* state,
                                     uint8_t command);

void haystack_command_encoder__poll(HaystackCommandEncoder_Config* config,
                                    HaystackCommandEncoder_State* state);

#endif
