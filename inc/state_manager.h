#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <freertos/FreeRTOSConfig.h>

 typedef enum global_state_t {
    IDLE,
    MENU,
    RUNNING_PROGRAM,
    NUM_STATES,
} global_state_t;



void state_manager_task(void* param);

void state_ir_receive_cb(void* event_data, void* user_data);
void sm_program_selected_cb(void* event_data, void* user_data);

#endif