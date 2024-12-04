#include "../inc/manager.h"
#include "../inc/timer_100ms.h"
#include "../inc/IR.h"
#include "../inc/state_manager.h"
#include "../inc/menu.h"
#include "../inc/display.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <esp_log.h>

#define TAG "Global Manager"

QueueHandle_t global_event_queue;
SemaphoreHandle_t cb_db_lock;

static cb_entry_t* event_cb_db[NUM_EVENTS] = {NULL};

void process_queue(void)
{
	system_event_t event;
	if(xQueueReceive(global_event_queue, &event, portMAX_DELAY)){ 
		notify_cbs(event.type, event.data);
				
		if(event.data != NULL && event.need_free)
		{
			ESP_LOGD(TAG, "Clearing event data");
			vPortFree(event.data);
		}
	}
}

void notify_cbs(event_type_t event_type, void* event_data)
{
	if(xSemaphoreTake(cb_db_lock, portMAX_DELAY) != pdTRUE)
	{
		ESP_LOGE(TAG, "Unable to obtain callback database mutex");
	}
	cb_entry_t* current = event_cb_db[event_type];

	while (current != NULL)
	{
		if(current->cb != NULL)
		{
			current->cb(event_data, current->user_data);
		}
		current = current->next;
	}
	xSemaphoreGive(cb_db_lock);
}

void register_cb(event_type_t event_type, void(*cb)(void*, void *), void* context_data, cb_entry_t** out_handle)
{
   if (event_type > NUM_EVENTS) {
		ESP_LOGE(TAG, "Event type not recoginized");
		return;
   }

   ESP_LOGD(TAG, "Registering cb for event %d", event_type);

   cb_entry_t* new_handle = (cb_entry_t*) pvPortMalloc(sizeof(cb_entry_t));
   if (new_handle == NULL)
   {
		ESP_LOGE(TAG, "Unable to allocate new_handle");
		return;
   }
   new_handle->user_data = context_data;
   new_handle->cb = cb;
   if(out_handle != NULL)
   {
		*out_handle = new_handle;
   }
	
	if(xSemaphoreTake(cb_db_lock, portMAX_DELAY) != pdTRUE)
	{
		ESP_LOGE(TAG, "Unable to obtain callback database mutex");
	}
   	new_handle->next = event_cb_db[event_type];
	event_cb_db[event_type] = new_handle;

	xSemaphoreGive(cb_db_lock);
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
				if(xSemaphoreTake(cb_db_lock, portMAX_DELAY) != pdTRUE)
				{
					ESP_LOGE(TAG, "Unable to obtain callback database mutex");
				}
				event_cb_db[event_type] = current->next;
				xSemaphoreGive(cb_db_lock);
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
	TaskHandle_t menu_task_handle;
	TaskHandle_t program_runner_task_handle;
	TaskHandle_t display_task_handle;

	global_event_queue = xQueueCreate(20, sizeof(system_event_t));
	cb_db_lock = xSemaphoreCreateMutex();

	setup_timer(&context);

	ESP_LOGD(TAG, "Starting IR Task");
	xTaskCreate(ir_task, "IR Task", 4096, NULL, 2, &IR_task_handle);

	ESP_LOGD(TAG, "Starting State Manager Task");
	xTaskCreate(state_manager_task, "State Manager Task", 4096, NULL, 2, &program_manager_handle);

	ESP_LOGD(TAG, "Starting Menu Task");
	xTaskCreate(menu_task, "Menu Task", 4096, NULL, 2, &menu_task_handle);

	ESP_LOGD(TAG, "Starting Program Runner");
	xTaskCreate(program_runner_task, "Program Runner Task", 4096, NULL, 1, &program_runner_task_handle);

	ESP_LOGD(TAG, "Starting Display Task");
	xTaskCreate(display_task, "Display Task", 4096, NULL, 1, &display_task_handle);

	while(1) {
		process_queue();
	}
}