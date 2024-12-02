#include "../inc/manager.h"
#include "../inc/timer_100ms.h"
#include "../inc/IR.h"
#include "../inc/program_manager.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <esp_log.h>


QueueHandle_t global_event_queue;
static SemaphoreHandle_t cb_count_sem;

static cb_entry_t* event_cb_db[NUM_EVENTS] = {NULL};

void process_queue(void)
{
    system_event_t event;
    if(xQueueReceive(global_event_queue, &event, portMAX_DELAY)){ // TODO: Don't think there's really any need for the switch case
        switch (event.type){
            case TIMER_TICK_100MS:
            case IR_CMD_RECEIVED:
            case TIMER_TICK_1S:
                notify_cbs(event.type, event.data);
                break;
            default:
            break;
        }

        while(uxSemaphoreGetCount(cb_count_sem) > 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        if(event.data != NULL)
        {
            vPortFree(event.data);
        }
    }
}

void notify_cbs(event_type_t event_type, void* event_data)
{
    cb_entry_t* current = event_cb_db[event_type];

    while (current != NULL)
    {
        if(current->cb != NULL)
        {
            xSemaphoreGive(cb_count_sem);
            current->cb(event_data, cb_count_sem);
        }
        current = current->next;
    }
}

void register_cb(event_type_t event_type, void(*cb)(void*, SemaphoreHandle_t), void* context_data)
{
   if (event_type > NUM_EVENTS) {
        ESP_LOGI("Manager", "Event type not recoginized");
        return;
   }
   ESP_LOGI("Manager", "Registering cb for event %d", event_type);

   cb_entry_t* new_handle = (cb_entry_t*) pvPortMalloc(sizeof(cb_entry_t));
   if (new_handle == NULL)
   {
    return;
   }
   new_handle->ctx_data = context_data;
   new_handle->next = event_cb_db[event_type];
   new_handle->cb = cb;

   event_cb_db[event_type] = new_handle;
}

void unregister_cb(event_type_t event_type, cb_entry_t* cb_handle)
{
    cb_entry_t* current = event_cb_db[event_type];
    cb_entry_t* prev = NULL;
    while (current != NULL)
    {
        if (current == cb_handle)
        {
            if(prev == NULL)
            {
                event_cb_db[event_type] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void master_task(void* param) {
    timer_context_t context;
    TaskHandle_t IR_task_handle;
    TaskHandle_t program_manager_handle;

    global_event_queue = xQueueCreate(20, sizeof(system_event_t));
    cb_count_sem = xSemaphoreCreateCounting(15, 0);
    setup_timer(&context);

    ESP_LOGI("Manager", "Starting IR Task");
    xTaskCreate(ir_task, "IR Task", 4096, NULL, 2, &IR_task_handle);
    ESP_LOGI("Manager", "Starting Program manager Task");
    xTaskCreate(program_manager_task, "Program Manager Task", 4096, NULL, 2, &program_manager_handle);
    while(1) {
        process_queue();
    }
}