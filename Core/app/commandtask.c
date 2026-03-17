#include "commandtask.h"
#include "cmsis_os2.h"
#include <stdint.h>
#include "freertos.h"
#include "projdefs.h"
#include <stdlib.h>
#include <sys/_intsup.h>
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"
#include "task.h"
#include "message_buffer.h"
#include "timers.h"
#include "stm32f4xx_hal_adc.h"
#include <stdio.h>
#include <string.h>

#include "rtc.h"
#include "kk_rtc.h"

uint8_t aRxBuffer[1];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart->Instance == USART2){
        osMessageQueuePut(CommandQueueHandle, &aRxBuffer[0], 0, 0);
        HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer, 1);//使能接收中断
        
    }
}





void StartCommandTask(void *argument){
    struct tm *now;
    char message[64];

    for (;;) {
          now = KK_RTC_GetTime();
          sprintf(message, "%d-%02d-%02d %02d:%02d:%02d\r\n", now->tm_year + 1900,now->tm_mon + 1,now->tm_mday,
          now->tm_hour,now->tm_min,now->tm_sec);
          HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
          HAL_Delay(1000);

    // osMessageQueueGet(CommandQueueHandle, &receive, 0, osWaitForever);
    // if (command_index == 0) {  // 如果命令索引为0，则寻找包头
    //     if (receive == 0xAA) {  // 找到包头
    //         command[command_index++] = receive;  // 命令索引加1,代表已经找到包头
    //     }
    // } else if (command_index == 1) {  // 如果命令索引为1，则receive为命令长度
    //     if (receive < 4 || receive > sizeof(command)) {
    //         command_index = 0;  // 重置命令索引
    //         continue;  // 命令长度不合法，重置命令索引，继续寻找包头
    //     }
    //     command_length = receive;
    //     command[command_index++] = receive;  // 命令索引加1,代表已经找到命令长度
    // } else {
    //     command[command_index++] = receive;
    //     if (command_index == command_length) {  // 如果命令索引等于命令长度，则命令接收完成 
    //         uint8_t checksum = 0;  // 校验和
    //         for (uint8_t i = 0; i < command_length - 1; i++) {
    //             checksum += command[i];
    //         }
    //         if (checksum == command[command_length - 1]) {  // 如果校验和等于命令长度，则命令接收完成
    //             for (uint8_t i = 2; i < command_length - 2; i += 2) {
    //                 LedMessage* msg = malloc(sizeof(LedMessage));
    //                 msg->color = command[i] - 1;
    //                 msg->state = command[i + 1];
    //                 osMessageQueuePut(LedQueueHandle, &msg, 0, osWaitForever);
    //             }
    //         }
    //         command_index = 0;  // 重置命令索引
    //         command_length = 0;  // 重置命令长度
    //     }
    // }
}

}
