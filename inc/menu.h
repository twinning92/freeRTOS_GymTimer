#ifndef MENU_H
#define MENU_H

#include <freertos/FreeRTOS.h>
#include "program.h"


void menu_task(void* param);
void menu_ir_receive_cb(void *event_data, void* user_data);
void menu_toggle_cb();

#endif