#pragma once

#include "usb.h"

void USB_PMAToMemory(uint8_t *mem, uint16_t offset, size_t length);
void USB_MemoryToPMA(uint16_t offset, const uint8_t *mem, size_t length);
void USB_SetEPR(volatile uint16_t *EPR, uint16_t status);
