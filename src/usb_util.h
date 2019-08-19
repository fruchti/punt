#pragma once

#include "usb.h"

static inline void USB_SetEPR(volatile uint16_t *EPR, uint16_t status)
{
    // Caution: This function does a read-modify-write and is prone to
    // unexpected behaviour when there are transactions going one, because the
    // register contents might change during the function's execution. Thus,
    // only use this function in initialisation code!
    volatile uint16_t v = *EPR;
    status ^= v & (USB_EP0R_DTOG_RX | USB_EP0R_STAT_RX |\
        USB_EP0R_DTOG_TX | USB_EP0R_STAT_TX);
    *EPR = status;
}

static inline void USB_SetEPRXStatus(volatile uint16_t *EPR, uint16_t status)
{
    uint16_t v = *EPR;
    v ^= status & USB_EP0R_STAT_RX;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX
        | USB_EP0R_EA | USB_EP0R_STAT_RX;
    *EPR = v;
}

static inline void USB_SetEPTXStatus(volatile uint16_t *EPR, uint16_t status)
{
    uint16_t v = *EPR;
    v ^= status & USB_EP0R_STAT_TX;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX
        | USB_EP0R_EA | USB_EP0R_STAT_TX;
    *EPR = v;
}

static inline void USB_SetEPType(volatile uint16_t *EPR, uint16_t type)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA;
    v |= USB_EP0R_EP_TYPE & type;
    *EPR = v;
}

static inline void USB_SetEPAddress(volatile uint16_t *EPR, uint16_t address)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND
        | USB_EP0R_CTR_TX;
    v |= USB_EP0R_EA & address;
    *EPR = v;
}

static inline void USB_SetEPKind(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_CTR_TX | USB_EP0R_EA;
    v |= USB_EP0R_EP_KIND;
    *EPR = v;
}

static inline void USB_ClearEPKind(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_CTR_TX | USB_EP0R_EA;
    *EPR = v;
}

static inline void USB_ClearCTRRX(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA;
    *EPR = v;
}

static inline void USB_ClearCTRTX(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_EA;
    *EPR = v;
}

static inline void USB_ClearDTOGRX(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_DTOG_RX | USB_EP0R_EP_TYPE
        | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX | USB_EP0R_EA;
    *EPR = v;
}

static inline void USB_ClearDTOGTX(volatile uint16_t *EPR)
{
    uint16_t v = *EPR;
    v &= USB_EP0R_CTR_RX | USB_EP0R_EP_TYPE | USB_EP0R_EP_KIND | USB_EP0R_CTR_TX
        | USB_EP0R_DTOG_TX | USB_EP0R_EA;
    *EPR = v;
}

void USB_PMAToMemory(uint8_t *mem, uint16_t offset, size_t length);
void USB_MemoryToPMA(uint16_t offset, const uint8_t *mem, size_t length);
