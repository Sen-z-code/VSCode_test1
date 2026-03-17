#ifndef __LEDTASK_H__
#define __LEDTASK_H__

#include "main.h"

typedef enum{
    red=0,
    green=1,
    yellow=2,
}ledcolor;

typedef enum{
    led_off=0,
    led_on=1,
}ledstate;

typedef struct{
    ledcolor color;
    ledstate state;
}LedMessage;

#endif
