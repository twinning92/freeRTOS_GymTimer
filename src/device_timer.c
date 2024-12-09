#include "../inc/device_timer.h"
#include "../inc/device_manager.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <stdbool.h>

timer_context_t gptimer;

bool gptimer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_ctx) {

    QueueHandle_t central_event_queue = (QueueHandle_t) user_ctx;
    BaseType_t high_task_wakeup = pdFALSE;

    system_event_t event_10ms = {
        .type = TIMER_TICK_10MS,
    };
    xQueueSendFromISR(central_event_queue, &event_10ms, &high_task_wakeup);
    
    if (event_data->count_value >= 1000000) {
        system_event_t event_1s = {
            .type = TIMER_TICK_1S,
        };
        xQueueSendFromISR(central_event_queue, &event_1s, &high_task_wakeup);
    }

    return high_task_wakeup == pdTRUE;
}

void setup_timer(QueueHandle_t central_event_queue) {

    gptimer.conf.clk_src = GPTIMER_CLK_SRC_APB;
    gptimer.conf.direction = GPTIMER_COUNT_UP;
    gptimer.conf.resolution_hz = 1000000;

    ESP_ERROR_CHECK(gptimer_new_timer(&gptimer.conf, &gptimer.gptimer_handle));

    gptimer.alarm_conf.alarm_count = 10000;
    gptimer.alarm_conf.reload_count = 0;
    gptimer.alarm_conf.flags.auto_reload_on_alarm = true;

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer.gptimer_handle, &gptimer.alarm_conf));

    gptimer.cbs.on_alarm = gptimer_isr_callback;

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer.gptimer_handle, &gptimer.cbs, (void *)central_event_queue));

    ESP_ERROR_CHECK(gptimer_enable(gptimer.gptimer_handle));
    ESP_ERROR_CHECK(gptimer_start(gptimer.gptimer_handle));
}

