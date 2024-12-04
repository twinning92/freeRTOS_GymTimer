#include "../inc/display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include <../inc/manager.h>

#define TAG "Display Manager"

void display_task(void *param)
{
    cb_entry_t *display_update_cb_handle;
    register_cb(DISPLAY_UPDATE, display_update_cb, NULL, &display_update_cb_handle);

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