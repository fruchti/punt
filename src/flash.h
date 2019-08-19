#pragma once

#include "stm32f103x6.h"

#define FLASH_PAGE_BYTES        1024U
#define FLASH_BOOTLOADER_PAGES  3U
#define FLASH_PAGES             64U

typedef enum
{
    FLASH_SUCCESS = 0,
    FLASH_PROHIBITED = 1,
    FLASH_VERIFY_FAILED = 2
} Flash_Status_t;

#define FLASH_APPLICATION_BASE  (FLASH_BASE \
                                + FLASH_BOOTLOADER_PAGES * FLASH_PAGE_BYTES)


Flash_Status_t Flash_ErasePage(int page);
void Flash_ProgramFromPMA(uint32_t flash_adress, uint16_t pma_offset,
    uint32_t length);