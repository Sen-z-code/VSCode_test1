#ifndef __KEYTASK_H__
#define __KEYTASK_H__

#define isKeyPressed() (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) == GPIO_PIN_RESET)

uint8_t iskeyclick(void);

#endif
