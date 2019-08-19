#include "buildinfo.h"
#include "flash.h"

const BuildInfo_t BuildInfo =
{
    .build_date = BUILD_DATE,
    .build_number = BUILD_NUMBER,
    .flash_application_start = FLASH_APPLICATION_BASE,
    .flash_application_size = (FLASH_PAGES - FLASH_BOOTLOADER_PAGES)
        * FLASH_PAGE_BYTES
};
