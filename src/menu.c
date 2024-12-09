#include <freertos/FreeRTOS.h>
#include <esp_log.h>

#include "../inc/menu.h"
#include "../inc/device_manager.h"
#include "../inc/IR.h"
#include "../inc/program.h"

#define TAG "Menu"

cbs_t menu_cbs = {
	.ir_cb = menu_ir_receive_cb,
	.tick1s_cb = menu_1s_receive_cb,
	.ticks10ms_cb = NULL,
};

volatile clock_state_t clock_state = IDLE;

void menu_task(void *param)
{
	QueueHandle_t central_event_queue = (QueueHandle_t) param;
	bool cbs_inserted = false;
	while(1)
	{
		switch(clock_state)
		{
			case IDLE:
				if(!cbs_inserted)
				{
					insert_cbs(&menu_cbs);
					cbs_inserted = true;
				}
			break;
			case RUNNING:
				cbs_inserted = false;

		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	

}

void program_shortcut(uint16_t cmd_val)
{
	switch(cmd_val)
	{
		case ONE:
			// program shortcut
		break;
		case TWO:
			// program shortcut
		break;
		case THREE:
			// program shortcut
		break;
		default:
			//etc
		break;
	}
}


bool menu_ir_receive_cb(uint16_t cmd_val, void* user_data) {
	switch(clock_state)
	{
		case IDLE:
			switch(cmd_val)
			{
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					clock_state = NAVIGATING;
				break;
				case STAR:
				case HASH:
					clock_state = STRINGS;
				break;
				default:
					program_shortcut(cmd_val);
					break;
			}
			break;
		case STRINGS:
			switch(cmd_val)
			{
				case UP:
					// string++
				break;
				case DOWN:
					// string--
				break;
				case STAR:
					// change string db
				break;
				case HASH:
					clock_state = IDLE;
				break;
				default:
					break;
			}
		case NAVIGATING:
			switch(cmd_val)
			{
				case UP:
				case RIGHT:
					//program index++
				break;
				case DOWN:
				case LEFT:
					//program index--
				break;
				case OKAY:
					// select
					clock_state = RUNNING;
				break;
				case STAR:
					clock_state = IDLE;
					break;
				default:
					break;
			}
			break;
	}
}
/**
 * @brief This cb exists purely to increment the clock.
 * 
 * @param user_data 
 * @return true 
 * @return false 
 */
bool menu_1s_receive_cb(void* user_data) {
	if(clock_state == IDLE)
	{
		// raise display update event with time to display
	}
}