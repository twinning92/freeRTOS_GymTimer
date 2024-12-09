#include "../inc/program.h"
#include "../inc/IR.h"
#include "../inc/device_manager.h"

#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include <string.h>
#define TAG "Program Manager"

static program_state_t prog_state = CONFIGURE;
program_config_t programs_db[NUM_PROGRAMS];

static volatile uint8_t launch_index;

static void init_programs();

void program_runner_task(void *param)
{
    QueueHandle_t central_event_queue = (QueueHandle_t) param;

    init_programs();

    while(1){
        vTaskDelay(pdMS_TO_TICKS(10));        
    }
}

void timer_1s_tick_cb(void* event_data, void* user_data)
{

}

void prog_ir_receive_cb(void* event_data, void* user_data)
{

}

void program_selected_cb(void* event_data, void* user_data)
{
    launch_index = *(uint8_t *)event_data;
}

void configure_program(uint8_t launch_index)
{
    program_config_t *program = pvPortMalloc(sizeof(program_config_t));
    memcpy(program, &programs_db[launch_index], sizeof(program_config_t));

    if(program->need_config){
        if(program->need_round)
        {
            //configure_rounds(program);
        }
        if(program->need_work)
        {
            //configure_work(program);
        }
        if(program->need_rest)
        {
            //configure_rest(program);
        }
        if(program->need_direction)
        {
            //configure_direction(program);
        }
    }
}


static void init_programs()
{
    programs_db[0].friendly_name = "up";
    programs_db[0].need_config = true;
    programs_db[0].need_round = false;
    programs_db[0].need_rest = false;
    programs_db[0].need_work = true;
    programs_db[0].count_direction = DIRECTION_UP;

    programs_db[1].friendly_name = "dn";
    programs_db[1].need_config = true;
    programs_db[1].need_round = false;
    programs_db[1].need_rest = false;
    programs_db[1].need_work = true;
    programs_db[1].count_direction = DIRECTION_DOWN;

    programs_db[2].friendly_name = "up rnd";
    programs_db[2].need_config = true;
    programs_db[2].need_round = true;
    programs_db[2].need_rest = false;
    programs_db[2].need_work = true;
    programs_db[2].count_direction = DIRECTION_UP;

    programs_db[3].friendly_name = "dn rnd";
    programs_db[3].need_config = true;
    programs_db[3].need_round = true;
    programs_db[3].need_rest = false;
    programs_db[3].need_work = true;
    programs_db[3].count_direction = DIRECTION_DOWN;

}