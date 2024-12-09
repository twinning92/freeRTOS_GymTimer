#ifndef MENU_H
#define MENU_H

#include <freertos/FreeRTOS.h>

/**
 * @brief The menu acts as the entry point to the state machine. When IDLE state (ie no inputs or programs running), time of day is displayed on the clock.
 * 
 */

extern clock_state_t clock_state;

typedef enum clock_state_t {
    IDLE,
    STRINGS,
    NAVIGATING,
    PROGRAM_RUNNING,
} clock_state_t;

void menu_task(void* param);
bool menu_ir_receive_cb(uint16_t cmd_val, void* user_data);
bool menu_1s_receive_cb(void *user_data);
void menu_toggle_cb();

#endif