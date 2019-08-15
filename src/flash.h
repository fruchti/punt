#pragma once

#include "stm32f103x6.h"

#define FLASH_START             0x08000000U
#define FLASH_PAGE_BYTES        1024U
#define FLASH_BOOTLOADER_PAGES  3U
#define FLASH_PAGES             64U

typedef enum
{
    FLASH_SUCCESS = 0,
    FLASH_PROHIBITED = 1,
    FLASH_VERIFY_FAILED = 2
} Flash_Status_t;

#define FLASH_APPLICATION_START (FLASH_START \
                                + FLASH_BOOTLOADER_PAGES * FLASH_PAGE_BYTES)


Flash_Status_t Flash_ErasePage(int page);