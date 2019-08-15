#pragma once

#include "stm32f103x6.h"

#include "usb.h"

void USB_HandleEP2Out(void);
void USB_HandleCommand(const USB_SetupPacket_t *sp);