#pragma once

#include "stm32f103x6.h"
#include "pinning.h"

#define LED_ON()                do { GPIOB->BSRR = 1 << PIN_LED; } while(0);
#define LED_OFF()               do { GPIOB->BRR = 1 << PIN_LED; } while(0);

void LED_Init(void);