#include "../lib/ir_nec.h"


static inline bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + IR_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - IR_NEC_DECODE_MARGIN)); 
}

static bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

static bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

static bool nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_REPEAT_CODE_DURATION_1);
}

bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t num_symbols, uint16_t* out_address, uint16_t* out_command)
{
    rmt_symbol_word_t *cur = rmt_nec_symbols;
    uint16_t address = 0;
    uint16_t command = 0;
    switch(num_symbols){
        case 34:
            bool valid_leading_code = nec_check_in_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                                    nec_check_in_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);
            if (!valid_leading_code) {
                return false;
            }
            cur++;
            for (int i = 0; i < 16; i++) {
                if (nec_parse_logic1(cur)) {
                    address |= 1 << i;
                } else if (nec_parse_logic0(cur)) {
                    address &= ~(1 << i);
                } else {
                    return false;
                }
                cur++;
            }
            for (int i = 0; i < 16; i++) {
                if (nec_parse_logic1(cur)) {
                    command |= 1 << i;
                } else if (nec_parse_logic0(cur)) {
                    command &= ~(1 << i);
                } else {
                    return false;
                }
                cur++;
            }
            *out_address = address;
            *out_command = command;
            break;
        case 2:
            *out_address = 0;
            *out_command = 0;
            break;
    }

    return true;
}


