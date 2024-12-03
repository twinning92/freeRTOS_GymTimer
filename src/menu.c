#include <freertos/FreeRTOS.h>
#include <esp_log.h>

#include "../inc/menu.h"
#include "../inc/manager.h"
#include "../inc/IR.h"
#include "../inc/program.h"

#define TAG "Menu"

static volatile ir_command_t menu_command;
static volatile bool enabled = false;

void menu_task(void *param)
{
	while(1){
		if(menu_command.new_data) {

		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void ir_received_cb(void *event_data, void* user_data)
{
	if(enabled && !menu_command.new_data){
		menu_command.next_command = *(uint16_t *)event_data;
		menu_command.new_data = true;
	}
}

void toggle_cb()
{
	enabled = !enabled;
}
