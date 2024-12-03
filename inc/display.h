#ifndef DISPLAY_H
#define DISPLAY_H

#include <freertos/FreeRTOS.h>

typedef struct display_t {
    char *display_string[6];
    uint16_t colour;
} display_t;


#endif