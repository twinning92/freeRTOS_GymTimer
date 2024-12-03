#ifndef MENU_H
#define MENU_H

#include <freertos/FreeRTOS.h>
#include "program.h"

void menu_task(void* param);
void ir_received_cb(void *event_data, void* user_data);
void toggle_cb();

#endif