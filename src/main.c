#include "main.h"

// Configure clocks based on a 8 MHz external crystal for:
// SYSCLK, AHB, APB2 72 Mhz
// APB1, ADC 36 MHz
static inline void Clock_Init(void)
{
    // Activate HSE and wait for it to be ready
    RCC->CR = RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));

    RCC->CFGR = RCC_CFGR_SW_0;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_0);

    FLASH->ACR |= FLASH_ACR_LATENCY_1;

    // Set PLL to x9 (-> 72MHz system clock)
    RCC->CFGR |= RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC | RCC_CFGR_PPRE1_2;

    // Activate PLL and wait
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    // Select PLL as clock source
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_1;

    // Disable all interrupts
    RCC->CIR = 0x00000000;
}

int main(void)
{
    Clock_Init();

    LED_Init();
    USB_Init();

    LED_OFF();

    for(;;)
    {
        USB_Poll();
    }
}

