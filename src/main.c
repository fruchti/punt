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

static inline bool Bootloader_EntryCondition(void)
{
    // Activate pull-up for test point
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH = (GPIOA->CRH
        & ~(0x0f << (PIN_TEST_POINT * 4 - 32)))
        | (0x08<< (PIN_TEST_POINT * 4 - 32))
        ;
    GPIOA->BSRR = (1 << PIN_TEST_POINT);

    // The first word in the application frame is the stack end pointer
    const uint32_t *application_start = (uint32_t*)FLASH_APPLICATION_BASE;
    // If it is not a valid RAM address, we can assume there's no application
    // present in flash and thus enter the bootloader
    if((*application_start & 0xffff8000) != 0x20000000)
    {
        return true;
    }

    // Check RTC backup register 1 for magic value
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;
    if(BKP->DR1 == 0xb007)
    {
        return true;
    }

    // Check if test point is held low externally
    if(~GPIOA->IDR & (1 << PIN_TEST_POINT))
    {
        return true;
    }

    return false;
}

static inline void Bootloader_Exit(void)
{
    // Reset RTC backup register
    BKP->DR1 = 0x0000;

    // Reset test point GPIO state to original
    GPIOA->CRH = 0x44444444;
    GPIOA->ODR = 0x00000000;

    // Reset peripheral clock enable registers to their reset values
    RCC->AHBENR = 0x00000014;   // Disable CRC
    RCC->APB2ENR = 0x00000000;  // Disable GPIOA, GPIOB, AFIO
    RCC->APB1ENR = 0x00000000;  // Disable USB, PWR, BKP

    // Set vector table location
    SCB->VTOR = FLASH_APPLICATION_BASE - FLASH_BASE;

    // Set stack pointer and jump into application
    uint32_t application_stack_pointer = *(uint32_t*)FLASH_APPLICATION_BASE;
    uint32_t application_reset_handler = *(uint32_t*)(FLASH_APPLICATION_BASE
        + 4);
    __asm__ volatile(".syntax unified \n"
        "msr msp, %[stack_pointer] \n"
        "bx %[reset_handler] \n"
        :
        : [stack_pointer] "r" (application_stack_pointer),
          [reset_handler] "r" (application_reset_handler));
}

int main(void)
{
    Clock_Init();

    if(Bootloader_EntryCondition())
    {
        USB_Init();

        while(USB_Poll());

        // Delay to allow answer token to be fetched by host
        Util_Delay(100000);

        // Reset USB peripheral
        RCC->APB1RSTR = RCC_APB1RSTR_USBRST;
        RCC->APB1RSTR = 0x00000000;
    }

    Bootloader_Exit();
}

