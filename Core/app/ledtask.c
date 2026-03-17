#include "cmsis_os2.h"
#include "ledtask.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdlib.h>
void StartLedTask(void *argument)
{
    for(;;)
    {
        LedMessage* msg;
        osMessageQueueGet(LedQueueHandle, &msg, 0, osWaitForever);
        switch (msg->color) {
        case red:
            HAL_GPIO_WritePin(LED_red_GPIO_Port, LED_red_Pin, msg->state?GPIO_PIN_SET:GPIO_PIN_RESET);
            break;
        case green:
            HAL_GPIO_WritePin(LED_green_GPIO_Port, LED_green_Pin, msg->state?GPIO_PIN_SET:GPIO_PIN_RESET);
            break;
        case yellow:
            HAL_GPIO_WritePin(LED_yellow_GPIO_Port, LED_yellow_Pin, msg->state?GPIO_PIN_SET:GPIO_PIN_RESET);
            break;
        default:
            break;
        }
        free(msg);
    }

}