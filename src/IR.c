#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <string.h>

#include "../inc/IR.h"
#include "../lib/ir_nec.h"
#include "../inc/device_manager.h"

#define TAG "IR Manager"

static rmt_symbol_word_t raw_symbols[64];
static rmt_rx_done_event_data_t rx_data;

static bool rx_received_cb(rmt_channel_handle_t rx_chan, const rmt_rx_done_event_data_t *edata, void *user_ctx){
    // decode then add to global queue?
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t *rx_queue = (QueueHandle_t*) user_ctx;

    xQueueSendFromISR(*rx_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}


static void ir_setup(rmt_channel_handle_t* rx_channel, QueueHandle_t* rx_queue, rmt_receive_config_t *receive_config) 
{
    ESP_LOGD(TAG, "create RMT RX channel");
    rmt_rx_channel_config_t rx_channel_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .mem_block_symbols = 64,
        .gpio_num = IR_RX_GPIO_NUM,
        .flags = {0},
    };

    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_config, rx_channel));
    ESP_LOGD(TAG, "register RX done callback");

    *rx_queue = xQueueCreate(3, sizeof(rmt_rx_done_event_data_t));

    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = rx_received_cb,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(*rx_channel, &cbs, rx_queue));

    ESP_LOGD(TAG, "Enabling rx channel");
    ESP_ERROR_CHECK(rmt_enable(*rx_channel));

    ESP_LOGD(TAG, "Starting RMT reception");
    ESP_ERROR_CHECK(rmt_receive(*rx_channel, raw_symbols, sizeof(raw_symbols), receive_config));

}


void ir_task(void *param)
{
    system_event_t ir_event = {
        .type = IR_CMD_RECEIVED,
    };

    rmt_channel_handle_t rx_channel_handle;
    QueueHandle_t rx_queue;
    QueueHandle_t central_event_queue = (QueueHandle_t) param;
    uint16_t address, command = 0;

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
    };

    ir_setup(&rx_channel_handle, &rx_queue, &receive_config);
    ESP_LOGD(TAG, "IR Task entering loop");

    while(1){
        if(xQueueReceive(rx_queue, &rx_data, portMAX_DELAY)) {          
            if(nec_parse_frame(rx_data.received_symbols, rx_data.num_symbols, &address, &command)){
                ESP_ERROR_CHECK(rmt_receive(rx_channel_handle, raw_symbols, sizeof(raw_symbols), &receive_config));
                ir_event.data = pvPortMalloc(sizeof(uint16_t));
                if(ir_event.data){
                    *(uint16_t *) ir_event.data = command;
                    ESP_LOGI(TAG, "command: 0x%04x", command);
                }
            xQueueSend(central_event_queue, &ir_event, 0);
            }
        }
    }
}