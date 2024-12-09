#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
/**
 * @brief Callback registration struct. Programs should populate and register this structure with the device manager on activation via the insert_cbs() method. Any cb not required should be set
 * to NULL. The active program will be responsible for the memory management.
 * 
 */
typedef struct cbs_t {
    bool (*ir_cb)(uint16_t cmd_val, void* usr_data);
    bool (*tick1s_cb)(void* usr_data);
    bool (*ticks10ms_cb)(void* usr_data);
} cbs_t;

typedef enum {
    TIMER_TICK_10MS,
    TIMER_TICK_1S,
    IR_CMD_RECEIVED,
    DISPLAY_UPDATE,
    NUM_EVENTS,
} event_type_t;

// struct to add to the global queue. Only free the pointer when need_free is set.
typedef struct {
    event_type_t type;
    void* data;
} system_event_t;

bool insert_cbs(cbs_t *insert_cbs);

void master_task(void *param);

#endif