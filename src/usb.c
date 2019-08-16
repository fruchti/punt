#include "usb.h"
#include "usb_descriptors.h"
#include "usb_util.h"
#include "usb_com.h"

#include "ownership.h"
MODULE_OWNS_PERIPHERAL(USB);
MODULE_OWNS_PIN(GPIOA, PIN_USB_PULLUP);
MODULE_OWNS_PIN(GPIOA, PIN_USB_DM);
MODULE_OWNS_PIN(GPIOA, PIN_USB_DP);

uint8_t USB_DeviceStatus[2] = {0x00, 0x01};
volatile unsigned int USB_ResetCount = 0;
volatile unsigned int USB_Address = 0;

static inline void USB_EnablePullup(void)
{
    GPIOA->BSRR = 1 << PIN_USB_PULLUP;
    GPIOA->CRH = (GPIOA->CRH
        & ~(0x0f << (PIN_USB_PULLUP * 4 - 32)))
        | (0x01 << (PIN_USB_PULLUP * 4 - 32))       // Push-pull output, 10 MHz
        ;    
}

static inline void USB_DisablePullup(void)
{
    GPIOA->BRR = 1 << PIN_USB_PULLUP;
    GPIOA->CRH = (GPIOA->CRH
        & ~(0x0f << (PIN_USB_PULLUP * 4 - 32)))
        | (0x04 << (PIN_USB_PULLUP * 4 - 32))       // Floating input
        ; 
}

void USB_Init(void)
{
    // GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
    RCC->AHBENR |= RCC_AHBENR_CRCEN;

    GPIOA->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_MODE11
        | GPIO_CRH_CNF12 | GPIO_CRH_MODE12);
    GPIOA->CRH |= GPIO_CRH_MODE11 | GPIO_CRH_MODE12;
    GPIOA->ODR &= ~(GPIO_CRH_MODE11 | GPIO_CRH_MODE12);
    USB_Delay(100000);

    // Analog power up
    USB->CNTR = (uint16_t)USB_CNTR_FRES;
    // Minimum delay: 1 µs
    USB_Delay(3000);

    USB->CNTR = (uint16_t)0;
    USB->ISTR = (uint16_t)0;
    USB->CNTR = (uint16_t)(USB_CNTR_RESETM | USB_CNTR_CTRM);
    
    // Free PA15 for use as GPIO
    AFIO->MAPR = (AFIO->MAPR
        & ~(AFIO_MAPR_SWJ_CFG))
        | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    USB_EnablePullup();

    // Configure USB pins (PA11 and PA12 in AF mode, 50 MHz push-pull)
    GPIOA->CRH |= GPIO_CRH_CNF11_1 | GPIO_CRH_MODE11
        | GPIO_CRH_CNF12_1 | GPIO_CRH_MODE12;

}

static inline void USB_HandleReset(void)
{
    // Remove reset flag
    USB->ISTR = (uint16_t)~(USB_ISTR_RESET);
    USB_ResetCount++;

    // Set buffer table origin
    USB->BTABLE = USB_BTABLE_OFFSET;

    // Control endpoint 0 (64 bytes size)
    USB_BTABLE_ENTRIES[0].COUNT_RX = USB_EP_RXCOUNT_BL_SIZE | (1 << 10);
    USB_BTABLE_ENTRIES[0].ADDR_RX = 0x40;
    USB_BTABLE_ENTRIES[0].COUNT_TX = 0;
    USB_BTABLE_ENTRIES[0].ADDR_TX = 0x80;

    USB_SetEPR(&(USB->EP0R), USB_EPR_EP_TYPE_CONTROL
        | USB_EPR_STAT_TX_NAK | USB_EPR_STAT_RX_VALID);

    // Endpoint 1: In (buffer size 64)
    USB_BTABLE_ENTRIES[1].COUNT_TX = 0;
    USB_BTABLE_ENTRIES[1].ADDR_TX = 0xc0;
    USB_BTABLE_ENTRIES[1].COUNT_RX = 0;
    USB_BTABLE_ENTRIES[1].ADDR_RX = 0;

    USB_SetEPR(&(USB->EP1R), USB_EPR_EP_TYPE_BULK
        | USB_EPR_STAT_TX_NAK | USB_EPR_STAT_RX_DISABLED
        | (1 << USB_EP1R_EA_Pos));

    // Endpoint 2: Out (buffer size 64)
    USB_BTABLE_ENTRIES[2].COUNT_RX = USB_EP_RXCOUNT_BL_SIZE | (1 << 10);
    USB_BTABLE_ENTRIES[2].ADDR_RX = 0x100;
    USB_BTABLE_ENTRIES[2].COUNT_TX = 0;
    USB_BTABLE_ENTRIES[2].ADDR_TX = 0;

    USB_SetEPR(&(USB->EP2R), USB_EPR_EP_TYPE_BULK
        | USB_EPR_STAT_TX_DISABLED | USB_EPR_STAT_RX_VALID
        | (2 << USB_EP2R_EA_Pos));

    // Enable
    USB->DADDR = USB_DADDR_EF;
}

static inline void USB_HandleIn(void)
{
    if((USB->DADDR & USB_DADDR_ADD) != USB_Address)
    {
        USB->DADDR = USB_Address | USB_DADDR_EF;
    }

    // Ready for next packet
    USB_SetEPRXStatus(&USB->EP0R, USB_EP_RX_VALID);
}


static inline void USB_HandleSetup(void)
{
    USB_SetupPacket_t sp;
    USB_PMAToMemory((uint8_t*)&sp, USB_BTABLE_ENTRIES[0].ADDR_RX,
        sizeof(USB_SetupPacket_t));

    // Delete this: Sanity check for abnormal setup package lengths
    int packet_length = USB_BTABLE_ENTRIES[0].COUNT_RX & 0x3ff;
    if(packet_length > sizeof(USB_SetupPacket_t))
    {
        __asm__ volatile("bkpt");
    }

    const uint8_t *reply_data = NULL;
    int reply_length = 0;
    uint8_t reply_response = USB_TOKEN_ACK;

    if((sp.bmRequestType & (USB_REQUEST_TYPE | USB_REQUEST_RECIPIENT))
        == (USB_REQUEST_TYPE_STANDARD | USB_REQUEST_RECIPIENT_DEVICE))
    {
        switch(sp.bRequest)
        {
            case USB_REQUEST_GET_STATUS:
                if(sp.wValue == 0 && sp.wIndex == 0 && sp.wLength == 2)
                {
                    reply_length = 2;
                    reply_data = USB_DeviceStatus;
                }
                break;

            case USB_REQUEST_GET_DESCRIPTOR:;
                USB_DescriptorType_t descriptor_type = sp.wValue >> 8;
                int descriptor_index = sp.wValue & 0xff;
                reply_length = sp.wLength;
                USB_HandleGetDescriptor(descriptor_type, descriptor_index,
                    &reply_data, &reply_length, &reply_response);
                break;
            
            case USB_REQUEST_SET_ADDRESS:
                USB_Address = sp.wValue & USB_DADDR_ADD;
                reply_response = USB_EP_TX_VALID;
                break;

            case USB_REQUEST_SET_CONFIGURATION:
                // Clear DTOG for all data endpoints
                USB_ClearDTOGTX(&(USB->EP1R));
                USB_ClearDTOGRX(&(USB->EP1R));
                USB_ClearDTOGTX(&(USB->EP2R));
                USB_ClearDTOGRX(&(USB->EP2R));

                reply_response = USB_EP_TX_VALID;
                break;

            default:
                reply_response = USB_EP_TX_STALL;
                break;
        }
    }
    else if((sp.bmRequestType & USB_REQUEST_TYPE) == USB_REQUEST_TYPE_VENDOR)
    {
        reply_response = USB_EP_TX_VALID;
        USB_HandleCommand(&sp);
    }
    else
    {
        // Unknown request
        reply_response = USB_EP_TX_STALL;
        __asm__ volatile("bkpt");
    }

    if(reply_data)
    {
        // Reply with data
        USB_MemoryToPMA(USB_BTABLE_ENTRIES[0].ADDR_TX, reply_data,
            reply_length);
        USB_BTABLE_ENTRIES[0].COUNT_TX = reply_length;
        USB_SetEPTXStatus(&(USB->EP0R), USB_EP_TX_VALID);
    }
    else
    {
        // Send response
        USB_BTABLE_ENTRIES[0].COUNT_TX = 0;
        USB_SetEPTXStatus(&(USB->EP0R), reply_response);
    }

    // Ready for the next packet
    USB_SetEPRXStatus(&(USB->EP0R), USB_EP_RX_VALID);
}

void USB_Poll(void)
{
    if(USB->ISTR & USB_ISTR_RESET)
    {
        // Reset happened
        USB_HandleReset();
        return;
    }
    uint16_t istr;
    while((istr = USB->ISTR) & (USB_ISTR_CTR))
    {
        if(istr & USB_ISTR_CTR)
        {
            // Correct transfer
            int ep = istr & USB_ISTR_EP_ID;
            switch(ep)
            {
                case 0:
                    // Determine transfer direction
                    if(istr & USB_ISTR_DIR)
                    {
                        // Out transfer
                        if(USB->EP0R & USB_EP0R_SETUP)
                        {
                            // Clear CTR_RX
                            USB_ClearCTRRX(&(USB->EP0R));

                            // Setup packed received
                            USB_HandleSetup();
                        }
                        else
                        {
                            // Only setup packets are supported, so other out
                            // transfers are just ignored

                            // Clear CTR_RX
                            USB_ClearCTRRX(&(USB->EP0R));
                        }
                    }
                    else
                    {
                        // In transfer

                        // Clear CTR_TX
                        USB_ClearCTRTX(&(USB->EP0R));

                        USB_HandleIn();
                    }
                    break;

                case 1:
                    // Data in endpoint
                    if(istr & USB_ISTR_DIR)
                    {
                        // Out transfer finished (shouldn't happen)
                        __asm__ volatile("bkpt");
                    }
                    else
                    {
                        // In transfer finished. STAT_TX gets set to NAK
                        // automatically.

                        // Clear CTR_TX
                        USB_BTABLE_ENTRIES[1].COUNT_TX = 0;
                        USB_ClearCTRTX(&(USB->EP1R));
                    }
                    break;

                case 2:
                    // Data out endpoint
                    if(istr & USB_ISTR_DIR)
                    {
                        // Clear CTR_RX
                        USB_ClearCTRRX(&(USB->EP2R));
                        USB_SetEPRXStatus(&(USB->EP2R), USB_EP_RX_VALID);

                        // Out transfer finished
                        USB_HandleEP2Out();
                    }
                    else
                    {
                        // In transfer finished (shouldn't happen)

                        __asm__ volatile("bkpt");
                    }
                    break;

                default:
                    // Other endpoints are not implemented
                    __asm__ volatile("bkpt");
                    break;
            }
        }
    }
}
