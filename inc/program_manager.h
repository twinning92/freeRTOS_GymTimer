#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include <freertos/FreeRTOSConfig.h>
#include <freertos/semphr.h>


typedef enum program_state_t {
    IDLE,
    MENU,
    CONFIG,
    READY,
    PRELIM,
    RUNNING,
    PAUSED,
    FINISHED,
} program_state_t;

typedef struct program_t {
    char name[10];
    uint8_t num_rounds;
} program_t;
 
typedef struct program_manager {
    program_state_t program_state;
    program_t selected_program;
};

void program_manager_task(void* param);

void ir_receive_cb(void* event_data, SemaphoreHandle_t cb_count_sem);
void clock_tick_cb(void *event_data, SemaphoreHandle_t cb_count_sem);

#endif