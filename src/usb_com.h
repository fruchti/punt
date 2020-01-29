#pragma once

#include <stdbool.h>
#include <string.h>
#include "stm32f103x6.h"

#include "usb.h"

void USB_HandleEP2Out(void);

// Parses a setup packet with a vendor request type and handle the command it
// contains. Returns false if the bootloader should exit and start the
// application, true if the bootloader should continue execution.
bool USB_HandleCommand(const USB_SetupPacket_t *sp);