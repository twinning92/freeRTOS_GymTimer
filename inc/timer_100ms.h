#ifndef TIMER_100MS_H
#define TIMER_100MS_H

#include <driver/gptimer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <stdbool.h>

typedef struct {
    gptimer_config_t conf;
    gptimer_alarm_config_t alarm_conf;
    gptimer_event_callbacks_t cbs;

    gptimer_handle_t gptimer_handle;
} timer_context_t;

void setup_timer(timer_context_t *context);
bool gptimer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_ctx);


#endif