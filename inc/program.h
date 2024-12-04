#ifndef PROGRAM_H
#define PROGRAM_H
#include <freertos/FreeRTOS.h>

#define NUM_PROGRAMS 4
#define DIRECTION_UP true
#define DIRECTION_DOWN false

typedef enum program_state_t {
    CONFIGURE,
    READY,
    COUNTDOWN,
    RUNNING,
    PAUSED,
    FINISHED,
} program_state_t;

typedef struct program_config_t {
    char* friendly_name;

    bool need_config;
    bool need_round;
    bool need_rest;
    bool need_work;
    bool need_direction;

    uint8_t num_rounds;
    uint32_t work_seconds;
    uint32_t rest_seconds;

    bool count_direction;

    void (*run_program)(void);
} program_config_t;

extern program_config_t programs_db[NUM_PROGRAMS];

void program_runner_task(void* param);
void timer_1s_tick_cb(void* event_data, void* user_data);
void prog_ir_receive_cb(void* event_data, void* user_data);
void program_selected_cb(void* event_data, void* user_data);
#endif