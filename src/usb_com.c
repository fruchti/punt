#include "usb_com.h"
#include "usb_util.h"
#include "commands.h"
#include "buildinfo.h"

static Command_t USB_PendingCommand = CMD_NOP;


void USB_HandleCommand(const USB_SetupPacket_t *sp)
{
    // The command is stored in the second byte (bRequest field= of the setup
    // packet
    Command_t command = sp->bRequest;

    const void *reply_data = NULL;
    int reply_length = 0;

    switch(command)
    {
        case CMD_BOOTLOADER_INFO:
            reply_data = &BuildInfo;
            reply_length = sizeof(BuildInfo);
            break;

        case CMD_READ_CRC:
            // The command will be executed as soon as the start address and
            // length are transferred via EP2
            USB_PendingCommand = CMD_READ_CRC;
            break;

        case CMD_READ_MEMORY:
            // The command will be executed as soon as the start address and
            // length are transferred via EP2
            USB_PendingCommand = CMD_READ_MEMORY;
            break;
        
        default:
            // Invalid commands get ignored
            break;
    }

    if(reply_length > 0)
    {
        // Reply with data via endpoint 1
        USB_MemoryToPMA(USB_BTABLE_ENTRIES[1].ADDR_TX, reply_data,
            reply_length);
        USB_BTABLE_ENTRIES[1].COUNT_TX = reply_length;

        USB_SetEPTXStatus(&(USB->EP1R), USB_EP_TX_VALID);
    }
}

void USB_HandleEP2Out(void)
{
    // Read how many bytes have been received by EP2
    int packet_length = USB_BTABLE_ENTRIES[2].COUNT_RX & 0x3ff;

    // Reply to be transmitted via EP1
    const void *reply_data = NULL;
    uint16_t reply_length = 0;

    uint32_t buff[2];

    // The meaning of the received data depends on the command transmitted via
    // a setup packet before it
    switch(USB_PendingCommand)
    {
        case CMD_READ_CRC:
            if(packet_length == 8)
            {
                USB_PMAToMemory((uint8_t*)buff, USB_BTABLE_ENTRIES[2].ADDR_RX, 8);
                uint8_t *start = (uint8_t*)(buff[0]);
                uint32_t length = buff[1];

                CRC->CR = CRC_CR_RESET;

                // TODO: Add basic sanity checks so it isn't possible to crash
                // the bootloader with this command (or at least not as easy)
                while(length--)
                {
                    CRC->DR = *start++;
                }

                buff[0] = CRC->DR;
                reply_data = buff;
                reply_length = 4;
            }
            break;


        case CMD_READ_MEMORY:
            if(packet_length == 8)
            {
                USB_PMAToMemory((uint8_t*)buff, USB_BTABLE_ENTRIES[2].ADDR_RX, 8);
                uint8_t *start = (uint8_t*)(buff[0]);
                uint32_t length = buff[1];

                if(length > 128)
                {
                    length = 128;
                }

                reply_data = start;
                reply_length = length;
            }
            break;

        default:
            break;
    }

    if(reply_length > 0)
    {
        // Reply with data via endpoint 1
        USB_MemoryToPMA(USB_BTABLE_ENTRIES[1].ADDR_TX, reply_data,
            reply_length);
        USB_BTABLE_ENTRIES[1].COUNT_TX = reply_length;

        USB_SetEPTXStatus(&(USB->EP1R), USB_EP_TX_VALID);
    }

    USB_PendingCommand = CMD_NOP;
}