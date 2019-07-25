#include "led.h"

#include "ownership.h"
MODULE_OWNS_PIN(GPIOB, PIN_LED);

void LED_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    GPIOB->CRL = (GPIOB->CRL
        & ~(0x0f << (PIN_LED * 4)))
        | (0x02 << (PIN_LED * 4))       // 2 MHz push-pull output
        ;
}