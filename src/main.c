#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include "../inc/device_manager.h"

void app_main(void){
    xTaskCreate(master_task, "Master Processor", 2048, NULL, 1, NULL);
}