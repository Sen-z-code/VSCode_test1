#include "cmsis_os2.h"
#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include "keytask.h"
#include "ledtask.h"

uint8_t key_state=0;
uint8_t key_judge=0;

uint8_t iskeyclick(void)
{
    switch (key_judge) {
    case 0:
        if(isKeyPressed()) key_judge = 1;
         break;
    case 1:
        if(isKeyPressed()) 
        {
            key_state = 1;
            key_judge = 2;
        }
        else key_judge = 0;
         break;
        case 2:
        if(!isKeyPressed()) key_judge = 0;
    default:
         break;
    }
    return key_state;
}



void StartKeyTask(void *argument)
{
    ledstate ledstate = led_off;
    for(;;)
    {
        if(iskeyclick())
        {
            key_state = 0;
            
            ledstate=!ledstate;
            LedMessage* msg=malloc(sizeof(LedMessage));
            msg->color = green;
            msg->state = ledstate;
            osMessageQueuePut(LedQueueHandle, &msg, 0, osWaitForever);
        }
        osDelay(10);
    }

}