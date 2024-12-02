#ifndef IR_H
#define IR_H

#include <freertos/FreeRTOS.h>

static const uint16_t UP    = 0xe718;
static const uint16_t DOWN  = 0xad52;
static const uint16_t LEFT  = 0xf708;
static const uint16_t RIGHT = 0xa55a;

static const uint16_t ONE   = 0xba45;
static const uint16_t TWO   = 0xb946;
static const uint16_t THREE = 0xb847;
static const uint16_t FOUR  = 0xbb44;
static const uint16_t FIVE  = 0xbf40;
static const uint16_t SIX   = 0xbc43;
static const uint16_t SEVEN = 0xf807;
static const uint16_t EIGHT = 0xea15;
static const uint16_t NINE  = 0xf609;
static const uint16_t ZERO  = 0xe619;

static const uint16_t STAR  = 0xe916;
static const uint16_t HASH  = 0xf20d;
static const uint16_t OKAY  = 0xe31c;


void ir_task(void* param);

#endif