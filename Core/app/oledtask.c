#include "oledtask.h"
#include "main.h"
#include "cmsis_os2.h"
#include "oled.h"
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

void StartOLEDTask(void *argument){
    char text[50];
    uint32_t count = 0;
    for(;;){
        sprintf(text,"%s","Hello World!");
        OLED_ShowString(1,1,text);
        sprintf(text,"%s","Hello World!");
        OLED_ShowNum(2,1,count++,10);
        // osDelay(10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

