#include "flash.h"

static inline void Flash_Unlock(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

static inline void Flash_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

Flash_Status_t Flash_ErasePage(int page)
{
    if(page < 0 || page >= FLASH_PAGES)
    {
        return FLASH_PROHIBITED;
    }
    uint32_t page_start = FLASH_START + page * FLASH_PAGE_BYTES;

    Flash_Unlock();
    while(FLASH->SR & FLASH_SR_BSY);
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = page_start;
    FLASH->CR |= FLASH_CR_STRT;
    while(FLASH->SR & FLASH_SR_BSY);
    Flash_Lock();

    // Verify
    for(uint32_t i = page_start; i < page_start + FLASH_PAGE_BYTES; i += 4)
    {
        if(*(uint32_t*)i != 0xffffffff)
        {
            return FLASH_VERIFY_FAILED;
        }
    }

    return FLASH_SUCCESS;
}