#ifndef MANAGER_H
#define MANAGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern QueueHandle_t global_event_queue;


typedef enum {
    TIMER_TICK_100MS,
    TIMER_TICK_1S,
    IR_CMD_RECEIVED,
    GLOBAL_STATE_CHANGE,
    DISPLAY_UPDATE,
    NUM_EVENTS,
} event_type_t;

typedef struct {
    event_type_t type;
    void* data;
} system_event_t;

typedef struct cb_entry_t {
    void (*cb)(void*, void*);
    void* user_data;
    struct cb_entry_t *next;
} cb_entry_t;

void process_queue(void);
void master_task(void* param);
void notify_cbs(event_type_t event_type,  void* event_data);
void register_cb(event_type_t event_type, void(*cb)(void*, void*), void* context_data, cb_entry_t** cb_handle);
void unregister_cb(event_type_t event_type, cb_entry_t* cb_handle);

#endif