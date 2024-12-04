#include <freertos/FreeRTOS.h>
#include <esp_log.h>

#include "../inc/menu.h"
#include "../inc/manager.h"
#include "../inc/IR.h"
#include "../inc/program.h"

#define TAG "Menu"

static volatile ir_command_t command;
static volatile bool cb_enabled = false;

void menu_task(void *param)
{
	cb_entry_t *ir_received_cb_handle;
	uint8_t program_index = 0;
	program_config_t display_index = programs_db[program_index];
	bool update_display = false;

	register_cb(IR_CMD_RECEIVED, menu_ir_receive_cb, NULL, &ir_received_cb_handle);
	system_event_t display_update = 
	{
		.type = DISPLAY_UPDATE,
		.need_free = false,
	};

	system_event_t program_select = 
	{
		.type = PROGRAM_SELECTED,
		.need_free = false,
	};
	while(1){
		if(command.queued) 
		{
			switch(command.value)
			{
				case UP:
				case RIGHT:
					program_index = (program_index + 1) % NUM_PROGRAMS;
				break;
				case DOWN:
				case LEFT:
					program_index = (program_index - 1 + NUM_PROGRAMS) % NUM_PROGRAMS;
				break;
				case OKAY:
					program_select.data = &program_index;
					xQueueSend(global_event_queue, &program_select, portMAX_DELAY);
					goto selected_program;
				break;
				default:
				break;
			}
		display_index = programs_db[program_index];
		update_display = true;
		}

		if(update_display)
		{
			display_update.data = display_index.friendly_name;
			xQueueSend(global_event_queue, &display_update, portMAX_DELAY);
			update_display = false;
		}
	selected_program:
		command.queued = false;
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void menu_ir_receive_cb(void *event_data, void* user_data)
{
	if(cb_enabled && !command.queued){
		command.value = *(uint16_t *)event_data;
		command.queued = true;
	}
}

void menu_toggle_cb()
{
	cb_enabled = !cb_enabled;
}
