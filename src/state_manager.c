#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <string.h>

#include "../inc/manager.h"
#include "../inc/state_manager.h"
#include "../inc/IR.h"
#include "../inc/display.h"
#include "../inc/menu.h"

#include <esp_log.h>

#define TAG "Program State Manager"

volatile global_state_t global_state = IDLE;
static volatile ir_command_t sm_command;
static volatile bool enabled = true;

static void change_state(global_state_t new_state)
{
    global_state = new_state;
    system_event_t event;
    event.data = pvPortMalloc(sizeof(global_state_t));
    event.type = GLOBAL_STATE_CHANGE;
    *(global_state_t*) event.data = new_state;
    xQueueSend(global_event_queue, &event, portMAX_DELAY);
}


void state_manager_task(void *param)
{
    cb_entry_t* ir_cb_handle;
    register_cb(IR_CMD_RECEIVED, ir_receive_cb, NULL, &ir_cb_handle);

    while(1)
    {
        if(sm_command.new_data)
        {
            ESP_LOGD(TAG, "Queue received");
            switch(global_state)
            {
                case IDLE:
                // Any user interaction from the remote, change to menu state. Could expand for quick jumps e.g. Press '2' to go to program X
                    change_state(MENU);
                break;
                case MENU:
                case RUNNING_PROGRAM:
                // 'STAR' should always return to IDLE
                if(sm_command.next_command == STAR)
                    change_state(IDLE);
                break;
                default:
                
                break;
            }
            sm_command.new_data = false;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void ir_receive_cb(void *event_data, void* user_data)
{
    if(enabled && !sm_command.new_data)
    {
        sm_command.next_command = *(uint16_t*) event_data;
        sm_command.new_data = true;
    }
}

void toggle_cb()
{
    enabled = !enabled;
}

