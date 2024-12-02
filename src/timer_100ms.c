#include "../inc/timer_100ms.h"
#include "../inc/manager.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <stdbool.h>

bool gptimer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_ctx) {
    static uint8_t tick_count = 0;
    BaseType_t high_task_wakeup = pdFALSE;

    system_event_t event_100ms = {
        .type = TIMER_TICK_100MS,
    };
    xQueueSendFromISR(global_event_queue, &event_100ms, &high_task_wakeup);
    
    tick_count++;
    if (tick_count >= 10) {
        system_event_t event_1s = {
            .type = TIMER_TICK_1S,
        };
        xQueueSendFromISR(global_event_queue, &event_1s, &high_task_wakeup);
        tick_count = 0;
    }

    return high_task_wakeup;
}

void setup_timer(timer_context_t* gptimer) {

    gptimer->conf.clk_src = GPTIMER_CLK_SRC_APB;
    gptimer->conf.direction = GPTIMER_COUNT_UP;
    gptimer->conf.resolution_hz = 1000000;

    ESP_ERROR_CHECK(gptimer_new_timer(&gptimer->conf, &gptimer->gptimer_handle));

    gptimer->alarm_conf.alarm_count = 100000;
    gptimer->alarm_conf.reload_count = 0;
    gptimer->alarm_conf.flags.auto_reload_on_alarm = true;

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer->gptimer_handle, &gptimer->alarm_conf));

    gptimer->cbs.on_alarm = gptimer_isr_callback;

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer->gptimer_handle, &gptimer->cbs, NULL));

    ESP_ERROR_CHECK(gptimer_enable(gptimer->gptimer_handle));
    ESP_ERROR_CHECK(gptimer_start(gptimer->gptimer_handle));
}

