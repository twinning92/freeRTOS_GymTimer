#include "../inc/display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include "../inc/device_manager.h"

#define TAG "Display Manager"

void display_task(void *param)
{
    QueueHandle_t central_event_queue = (QueueHandle_t) param;

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void display_update_cb(void* event_data, void* user_data)
{
    char* string = (char*) event_data;
    ESP_LOGI(TAG, "%s", string);

}