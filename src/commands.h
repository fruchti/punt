#pragma once

// Legend:
//  ->  is a transmission via endpoint 2 (out) from the host to the MCU
//  <-  is a transmission via endpoint 1 (in) from the MCU to the host

typedef enum
{
    // Does nothing
    CMD_NOP = 0x00,

    // Basic bootloader info. Returns 19 to 64 bytes (cf. BootloaderInfo_t):
    //  <-  Build date: u32 (YYYYMMDD as an unsigned integer)
    //  <-  Build number: u32
    //  <-  Flash application base address: u32
    //  <-  Maximum application size: u32
    //  <-  Major version: u8
    //  <-  Minor version: u8
    //  <-  Patch version: u8
    //  <-  Identifier string (variable length, not zero-terminated)
    CMD_BOOTLOADER_INFO = 0x01,

    // Calculate a CRC32 of a memory region:
    //  ->  Start address: u32
    //  ->  Length: u32
    //  <-  CRC32: u32
    CMD_READ_CRC = 0x02,

    // Read memory contents
    //  ->  Start address: u32
    //  ->  Length n: u32
    //  <-  Data: n bytes
    CMD_READ_MEMORY = 0x03,

    // Erase a single 1 KiB flash page
    //  ->  Page number: u8
    //  <-  Return code (0 for success): u8
    CMD_ERASE_PAGE = 0x04,

    // Program flash
    //  ->  Start adress: u32
    //  ->  Data: n bytes (n must be even)
    CMD_PROGRAM = 0x05,

    // Exit bootloader and start application software
    CMD_EXIT = 0xff
} Command_t;

typedef struct
{
    uint32_t build_date;
    uint32_t build_number;
    uint32_t flash_application_start;
    uint32_t flash_application_size;
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_patch;
    char identifier[];
} __attribute__((packed, aligned(1))) BootloaderInfo_t;
