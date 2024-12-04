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
static volatile bool cb_enabled = true;

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
    cb_entry_t* program_selected_cb_handle;
    register_cb(IR_CMD_RECEIVED, state_ir_receive_cb, NULL, &ir_cb_handle);
    register_cb(PROGRAM_SELECTED, sm_program_selected_cb, NULL, &program_selected_cb_handle);

    while(1)
    {
        if(sm_command.queued)
        {
            switch(global_state)
            {
                case IDLE:
                // Any user interaction from the remote, change to menu state. Could expand for quick jumps e.g. Press '2' to go to program X
                    menu_toggle_cb();
                    change_state(MENU);
                break;
                case MENU:
                    // 'STAR' should always return to IDLE
                    if(sm_command.value == STAR){
                        menu_toggle_cb();
                        change_state(IDLE);
                    }
                break;
                case RUNNING_PROGRAM:
                    // 'STAR' should always return to IDLE
                    if(sm_command.value == STAR){
                        menu_toggle_cb();
                        change_state(IDLE);
                    }
                break;
                default:
                
                break;
            }
            sm_command.queued = false;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void state_ir_receive_cb(void *event_data, void* user_data)
{
    if(cb_enabled && !sm_command.queued)
    {
        sm_command.value = *(uint16_t*) event_data;
        sm_command.queued = true;
    }
}

void sm_program_selected_cb(void* event_data, void* user_data)
{
    if(event_data != NULL)
    {
        menu_toggle_cb();
        change_state(RUNNING_PROGRAM);
    }

}
