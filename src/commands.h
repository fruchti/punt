#pragma once

typedef enum
{
    // Basic bootloader info. Returns 8 bytes:
    //  <-  Build date: u32 (YYYYMMDD as an unsigned integer)
    //  <-  Build number: u32
    CMD_BOOTLOADER_INFO = 0x01,
} Command_t;