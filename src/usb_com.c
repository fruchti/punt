#include "usb_com.h"
#include "usb_util.h"
#include "commands.h"
#include "buildinfo.h"
#include "flash.h"

static Command_t USB_PendingCommand = CMD_NOP;


void USB_HandleCommand(const USB_SetupPacket_t *sp)
{
    // The command is stored in the second byte (bRequest field) of the setup
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
        
        case CMD_ERASE_PAGE:
            // The command will be executed as soon as the page number is
            // transferred via EP2. Since only a single byte is needed for the
            // page index, this would also be technically be possible with just
            // one setup packet. Since this is the only command, this minor
            // USB bandwith saving is not worth the extra special case.
            USB_PendingCommand = CMD_ERASE_PAGE;
            break;

        case CMD_PROGRAM:
            USB_PendingCommand = CMD_PROGRAM;
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

void USB_EP1Transmit(void *data, uint16_t length)
{
    USB_MemoryToPMA(USB_BTABLE_ENTRIES[1].ADDR_TX, data, length);
    USB_BTABLE_ENTRIES[1].COUNT_TX = length;

    USB_SetEPTXStatus(&(USB->EP1R), USB_EP_TX_VALID);
}

void USB_HandleEP2Out(void)
{
    // Read how many bytes have been received by EP2
    int packet_length = USB_BTABLE_ENTRIES[2].COUNT_RX & 0x3ff;

    // The meaning of the received data depends on the command transmitted via
    // a setup packet before it
    switch(USB_PendingCommand)
    {
        case CMD_READ_CRC:
            if(packet_length == 8)
            {
                uint32_t buff[2];
                USB_PMAToMemory((uint8_t*)buff, USB_BTABLE_ENTRIES[2].ADDR_RX,
                    sizeof(buff));
                uint32_t *addr = (uint32_t*)(buff[0]);
                uint32_t length = buff[1];

                CRC->CR = CRC_CR_RESET;

                // TODO: Add basic sanity checks so it isn't possible to crash
                // the bootloader with this command (or at least not as easy)
                while(length > 4)
                {
                    CRC->DR = *addr++;
                    length -= 4;
                }
                CRC->DR = *addr & (0xffffffffU >> (32 - 8 * length));

                buff[0] = CRC->DR;
                USB_EP1Transmit(buff, 4);
            }
            break;

        case CMD_READ_MEMORY:
            if(packet_length == 8)
            {
                uint32_t buff[2];
                USB_PMAToMemory((uint8_t*)buff, USB_BTABLE_ENTRIES[2].ADDR_RX,
                    sizeof(buff));
                uint8_t *start = (uint8_t*)(buff[0]);
                uint32_t length = buff[1];

                if(length > 64)
                {
                    length = 64;
                }

                USB_EP1Transmit(start, length);
            }
            break;

        case CMD_ERASE_PAGE:
            if(packet_length == 1)
            {
                // Not that only one byte has been received but since the PMA
                // can only be accessed word-wise, we'll have to copy two
                uint8_t buff[2];
                USB_PMAToMemory(buff, USB_BTABLE_ENTRIES[2].ADDR_RX,
                    sizeof(buff));

                int page_index = buff[0];
                if(page_index < FLASH_BOOTLOADER_PAGES)
                {
                    // Do not allow erasing the bootloader
                    buff[0] = FLASH_PROHIBITED;
                }
                else
                {
                    buff[0] = Flash_ErasePage(page_index);
                }

                // Reply with status byte
                USB_EP1Transmit(buff, 1);
            }
            break;

        case CMD_PROGRAM:;
            uint32_t start;
            USB_PMAToMemory((uint8_t*)&start, USB_BTABLE_ENTRIES[2].ADDR_RX, 4);
            uint32_t length = packet_length - 4;
            if(start >= FLASH_APPLICATION_START && start + length
                <= FLASH_BASE + FLASH_PAGE_BYTES * FLASH_PAGES)
            {
                // Program directly from PMA without an intermediate buffer in
                // RAM
                Flash_ProgramFromPMA(start, USB_BTABLE_ENTRIES[2].ADDR_RX + 4,
                    length);
            }
            break;

        default:
            break;
    }

    USB_PendingCommand = CMD_NOP;
}
