#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include "../inc/manager.h"
#include "../inc/program_manager.h"
#include "../inc/IR.h"

#include <esp_log.h>

void program_manager_task(void *param)
{    
    register_cb(IR_CMD_RECEIVED, ir_receive_cb, NULL);
    register_cb(TIMER_TICK_1S, clock_tick_cb, NULL);

    while(1)
    {
        vTaskDelay(100);
    }
}

void ir_receive_cb(void *event_data, SemaphoreHandle_t cb_count_sem)
{
    if (event_data == NULL)
    {
        ESP_LOGI("PM", "No event data provided for IR receive");
    }
    else
    {
        ESP_LOGI("PM", "Command= 0x%04x", *(uint16_t*) event_data);
    }
    xSemaphoreTake(cb_count_sem, portMAX_DELAY);

}



void clock_tick_cb(void *event_data, SemaphoreHandle_t cb_count_sem)
{
    xSemaphoreTake(cb_count_sem, portMAX_DELAY);
}