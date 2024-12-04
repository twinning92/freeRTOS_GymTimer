#ifndef DISPLAY_H
#define DISPLAY_H

#include <freertos/FreeRTOS.h>

void display_task(void* param);
void display_update_cb(void* event_data, void* user_data);


#endif