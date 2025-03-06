#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

void Error_Handler(void);

#define DHT11_Pin GPIO_PIN_14
#define DHT11_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif
