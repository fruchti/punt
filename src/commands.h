#pragma once

// Legend:
//  ->  is a transmission via endpoint 2 (out) from the host to the MCU
//  <-  is a transmission via endpoint 1 (in) from the MCU to the host

typedef enum
{
    // Does nothing
    CMD_NOP = 0x00,

    // Basic bootloader info. Returns 8 bytes:
    //  <-  Build date: u32 (YYYYMMDD as an unsigned integer)
    //  <-  Build number: u32
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
} Command_t;