#ifndef IR_NEC_H
#define IR_NEC_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/rmt_rx.h"

#define IR_RESOLUTION_HZ     1000000 
#define IR_RX_GPIO_NUM       GPIO_NUM_19
#define IR_NEC_DECODE_MARGIN 200    

#define NEC_LEADING_CODE_DURATION_0  9000
#define NEC_LEADING_CODE_DURATION_1  4500
#define NEC_PAYLOAD_ZERO_DURATION_0  560
#define NEC_PAYLOAD_ZERO_DURATION_1  560
#define NEC_PAYLOAD_ONE_DURATION_0   560
#define NEC_PAYLOAD_ONE_DURATION_1   1690
#define NEC_REPEAT_CODE_DURATION_0   9000
#define NEC_REPEAT_CODE_DURATION_1   2250

bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t num_symbols, uint16_t* out_address, uint16_t* out_command);

#endif