#include "usb_util.h"

void USB_PMAToMemory(uint8_t *mem, uint16_t offset, size_t length)
{
    // Only words can be copied. Thus, if the length is not even, it has to be
    // incremented to ensure that the last byte is copied. This of course means
    // that the target memory area must be of even length!
    if(length & 1)
    {
        length++;
    }

    uint8_t *pma = (uint8_t*)(USB_PMA_ADDR + 2 * offset);
    for(unsigned int i = 0; i < length / 2; i++)
    {
        mem[2 * i] = *pma++;
        mem[2 * i + 1] = *pma++;
        pma += 2;
    }
}

void USB_MemoryToPMA(uint16_t offset, const uint8_t *mem, size_t length)
{
    // Only words can be copied. Thus, if the length is not even, it has to be
    // incremented to ensure that the last byte is copied. Since the PMA buffer
    // always has even size, this is not a problem.
    if(length & 1)
    {
        length++;
    }

    uint16_t *pma = (uint16_t*)(USB_PMA_ADDR + 2 * offset);
    for(unsigned int i = 0; i < length / 2; i++)
    {
        uint16_t tmp = mem[2 * i] | (mem[2 * i + 1] << 8);
        *pma++ = tmp;
        pma++;
    }
}

void USB_SetEPR(volatile uint16_t *EPR, uint16_t status)
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