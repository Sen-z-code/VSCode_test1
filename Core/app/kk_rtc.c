#include "kk_rtc.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

// RTC已经被初始化的值 记录在RTC_BKP_DR1中
#define RTC_INIT_FLAG 0x2333

// 使用静态结构体，防止返回局部变量的指针
static struct tm currentTime;

/**
 * @brief 设置RTC时间
 * @param time 指向要设置的 C 标准 tm 时间结构体
 * @retval HAL status
 */
HAL_StatusTypeDef KK_RTC_SetTime(struct tm *time){
    /* 目的：
       - 将标准 C `struct tm` 转换为 STM32F4 的 RTC_DateTypeDef / RTC_TimeTypeDef
       - 调用 HAL_RTC_SetDate / HAL_RTC_SetTime 完成写入
       注意：
       - HAL RTC SetDate/SetTime 返回值非常重要，必须检查返回值以确认写入是否成功
       - 如果 RTC 配置为使用预装载（preload），写入可能写入影子寄存器，实际生效在下一个更新事件（UG）
       - 如果 RTC 硬件未被正确初始化或 LSE/时钟源不可用，写入会失败
    */
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /* 年份：RTC 只保存 0-99（表示 2000-2099），所以做 %100 或减去 1900/2000 的转换 */
    sDate.Year = time->tm_year % 100;
    sDate.Month = time->tm_mon + 1; // tm:0-11 -> RTC:1-12
    sDate.Date = time->tm_mday;

    sTime.Hours = time->tm_hour;
    sTime.Minutes = time->tm_min;
    sTime.Seconds = time->tm_sec;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    /* 写日期 -> 写时间。先写日期是常见做法（保证读回时一致）。
       返回值应当返回给调用者用于判断是否成功。 */
    HAL_StatusTypeDef status;
    status = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    if (status != HAL_OK) return status;
    status = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    return status;
}

/**
 * @brief 获取RTC时间
 * @retval 时间结构体指针
 */
struct tm *KK_RTC_GetTime() {
    /* 目的：
       - 用 HAL RTC GetTime + GetDate 读取当前硬件日历
       - STM32F4 要先读 Time 再读 Date（或者相反但必须按 HAL 指南），以确保影子寄存器同步且读取一致
       - 将 RTC 的年/月/日/时/分/秒 转换回 C 的 struct tm
    */
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // 对于STM32F4，读取时间时必须既读Time又读Date，从而解锁日历的影子寄存器
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    /* tm_year 存储的是自 1900 年起的年数，RTC 的 Year 是 0-99 表示 2000-2099 */
    currentTime.tm_year = sDate.Year + 100;
    currentTime.tm_mon = sDate.Month - 1;
    currentTime.tm_mday = sDate.Date;
    currentTime.tm_hour = sTime.Hours;
    currentTime.tm_min = sTime.Minutes;
    currentTime.tm_sec = sTime.Seconds;

    return &currentTime;
}

/**
 * @brief RTC 初始化及判断是否掉电复位
 */
void KK_RTC_Init(){
    /* 重要顺序：
       - 必须先使能 PWR 时钟并允许备份域写访问（HAL_PWR_EnableBkUpAccess）
       - 检查 Backup Register 标志（用于判定是否已完成一次初始化）
       - 仅在设置时间成功时才写入备份寄存器（避免写入标志但时间未写成功）
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    uint32_t initFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    if(initFlag == RTC_INIT_FLAG) return; // 已初始化过，直接返回

    /* 如果未初始化，设置默认时间并在成功时写入备份寄存器 */
    struct tm time = { .tm_year = 2025-1900, .tm_mon = 0, .tm_mday = 1, .tm_hour = 23, .tm_min = 59, .tm_sec = 55 };

    HAL_StatusTypeDef st = KK_RTC_SetTime(&time);
    if (st == HAL_OK) {
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_INIT_FLAG);
    } else {
        /* 出错时打印或采取恢复措施，方便定位（请确认 huart2 可用） */
        char msg[64];
        sprintf(msg, "KK_RTC_SetTime failed: %d\r\n", (int)st);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
}