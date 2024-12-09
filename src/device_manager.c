#include "../inc/device_manager.h"
#include "../inc/device_timer.h"
#include "../inc/IR.h"
#include "../inc/menu.h"
#include "../inc/display.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <esp_log.h>

#define TAG "Device Manager"

QueueHandle_t central_event_queue;
cbs_t* active_cbs;

static void notify_cbs(event_type_t event_type, void* event_data)
{
	bool ret = false;
	void* usr_data = NULL; // Not sure if this is even necessary?
	if(active_cbs != NULL)
	{
		switch(event_type)
		{
			case TIMER_TICK_10MS:
				ret = active_cbs->ticks10ms_cb(usr_data);
				break;
			case TIMER_TICK_1S:
				ret = active_cbs->tick1s_cb(usr_data);
				break;
			case IR_CMD_RECEIVED:
				ret = active_cbs->ir_cb(*(uint16_t*) event_data, usr_data);
				break;
		}
	}
	else
	{
		ESP_LOGE(TAG, "active_cbs is NULL");
	}
}

bool insert_cbs(cbs_t* insert_cbs)
{
	if(insert_cbs != NULL)
	{
		active_cbs = insert_cbs;
	}
}

static void process_queue(void)
{
	system_event_t event;
	if(xQueueReceive(central_event_queue, &event, portMAX_DELAY)){
		if (event.type == DISPLAY_UPDATE)
		{
			// Update the display from here.
		}
		else
		{
			notify_cbs(event.type, event.data);
		}		
		if(event.data != NULL)
		{
			ESP_LOGD(TAG, "Clearing event data");
			vPortFree(event.data);
		}
	}
}

void master_task(void* param) {
	
	TaskHandle_t IR_task_handle;
	TaskHandle_t menu_task_handle;
	TaskHandle_t program_runner_task_handle;
	TaskHandle_t display_task_handle;

	central_event_queue = xQueueCreate(20, sizeof(system_event_t));

	setup_timer(central_event_queue);

	ESP_LOGD(TAG, "Starting IR Task");
	xTaskCreate(ir_task, "IR Task", 4096, central_event_queue, 2, &IR_task_handle);

	ESP_LOGD(TAG, "Starting Menu Task");
	xTaskCreate(menu_task, "Menu Task", 4096, central_event_queue, 2, &menu_task_handle);

	ESP_LOGD(TAG, "Starting Program Runner");
	xTaskCreate(program_runner_task, "Program Runner Task", 4096, central_event_queue, 1, &program_runner_task_handle);

	ESP_LOGD(TAG, "Starting Display Task");
	xTaskCreate(display_task, "Display Task", 4096, central_event_queue, 1, &display_task_handle);

	while(1) {
		process_queue();
	}
}