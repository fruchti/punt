#include "usb_descriptors.h"

const USB_DeviceDescriptor_t USB_DeviceDescriptor =
{
    .bLength = 18,
    .bDescriptorType = USB_DEVICE_DESCRIPTOR,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0xff,
    .bDeviceSubClass = 0xff,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x16c0,
    .idProduct = 0x05dc,
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1
};

const USB_WholeDescriptor_t USB_ConfigurationInterfaceDescriptor =
{
    .configuration = (USB_ConfigurationDescriptor_t)
    {
        .bLength = 9,
        .bDescriptorType = USB_CONFIGURATION_DESCRIPTOR,
        .wTotalLength = sizeof(USB_WholeDescriptor_t),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = 0x80,
        .bMaxPower = 100
    },

    .main_interface = (USB_InterfaceDescriptor_t)
    {
        .bLength = 9,
        .bDescriptorType = USB_INTERFACE_DESCRIPTOR,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = 0x00,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0
    },

    // Endpoint 1: in
    .data_in_endpoint = (USB_EndpointDescriptor_t)
    {
        .bLength = sizeof(USB_EndpointDescriptor_t),
        .bDescriptorType = USB_ENDPOINT_DESCRIPTOR,
        .bEndpointAddress = USB_ENDPOINT_IN | 1,
        .bmAttributes = USB_ENDPOINT_BULK | USB_ENDPOINT_NO_SYNCHRONIZATION
            | USB_ENDPOINT_DATA,
        .wMaxPacketSize = 64,
        .bInterval = 0x01
    },

    // Endpoint 2: out
    .data_out_endpoint = (USB_EndpointDescriptor_t)
    {
        .bLength = sizeof(USB_EndpointDescriptor_t),
        .bDescriptorType = USB_ENDPOINT_DESCRIPTOR,
        .bEndpointAddress = USB_ENDPOINT_OUT | 2,
        .bmAttributes = USB_ENDPOINT_BULK | USB_ENDPOINT_NO_SYNCHRONIZATION
            | USB_ENDPOINT_DATA,
        .wMaxPacketSize = 64,
        .bInterval = 0x01
    },
};

#define USB_STRING_LANGID                       0x0409
#define USB_STRING_VENDOR                       0
#define USB_STRING_PRODUCT                      'P', 'u', 'n', 't', 0
#define USB_STRING_SERIAL                       0

const uint16_t USB_StringDescriptor_LangID[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_LANGID);
const uint16_t USB_StringDescriptor_Vendor[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_VENDOR);
const uint16_t USB_StringDescriptor_Product[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_PRODUCT);
const uint16_t USB_StringDescriptor_Serial[] =
    USB_BUILD_STRING_DESCRIPTOR(USB_STRING_SERIAL);

void USB_HandleGetDescriptor(USB_DescriptorType_t descriptor_type,
    int descriptor_index, const uint8_t **reply_data, int *reply_length,
    uint8_t *reply_response)
{
    switch(descriptor_type)
    {
        case USB_DEVICE_DESCRIPTOR:
            *reply_data = USB_DeviceDescriptor.raw;
            *reply_length = USB_DeviceDescriptor.bLength;
            break;

        case USB_CONFIGURATION_DESCRIPTOR:
            *reply_data = USB_ConfigurationInterfaceDescriptor.raw;
            if(*reply_length < USB_ConfigurationInterfaceDescriptor
                .configuration.wTotalLength)
            {   
                *reply_length = USB_ConfigurationInterfaceDescriptor
                    .configuration.bLength;
            }
            else
            {
                *reply_length = USB_ConfigurationInterfaceDescriptor
                    .configuration.wTotalLength;
            }
            break;

        case USB_STRING_DESCRIPTOR:
            switch(descriptor_index)
            {
                case 0:
                    *reply_data = (uint8_t*)USB_StringDescriptor_LangID;
                    *reply_length = (uint8_t)*USB_StringDescriptor_LangID;
                    break;
                case 1:
                    *reply_data = (uint8_t*)USB_StringDescriptor_Vendor;
                    *reply_length = (uint8_t)*USB_StringDescriptor_Vendor;
                    break;
                case 2:
                    *reply_data = (uint8_t*)USB_StringDescriptor_Product;
                    *reply_length = (uint8_t)*USB_StringDescriptor_Product;
                    break;
                case 3:;
                    // String descriptors are 16 bits per char
                    static uint16_t buff[26];
                    // The first byte is the total length in bytes, the second
                    // byte is the descriptor type (3)
                    buff[0] = (0x03 << 8) | sizeof(buff);
                    // The unique device ID is 96 bits = 12 bytes long
                    for(int i = 0; i < 12; i++)
                    {
                        uint8_t uid_byte = *((uint8_t*)UID_BASE + i);
                        // The representation does not matter for the serial, so
                        // we're using one of the first 16 letters of the
                        // alphabet for each nibble
                        buff[1 + 2 * i] = 'A' + (uid_byte & 0x0f);
                        buff[2 + 2 * i] = 'A' + (uid_byte >> 4);
                    }
                    // Null-terminate the string
                    buff[sizeof(buff) / 2 - 1] = 0;
                    *reply_data = (uint8_t*)buff;
                    *reply_length = (uint8_t)*buff;
                    break;
                default:
                    __asm__ volatile("bkpt");
            }
            break;

        case USB_INTERFACE_DESCRIPTOR:
            switch(descriptor_index)
            {
                case 0:
                    *reply_data = USB_ConfigurationInterfaceDescriptor
                            .main_interface.raw;
                    *reply_length = USB_ConfigurationInterfaceDescriptor
                            .main_interface.bLength;
                            
                default:
                    __asm__ volatile("bkpt");
            }
            break;

        case USB_DEVICE_QUALIFIER_DESCRIPTOR:
            // Device is full-speed only, so it must return a request error
            *reply_response = USB_EP_TX_STALL;
            *reply_data = NULL;
            break;

        case USB_ENDPOINT_DESCRIPTOR:
        case USB_OTHER_DESCRIPTOR:
        case USB_INTERFACE_POWER_DESCRIPTOR:
        case USB_INTERFACE_ASSOCIATION_DESCRIPTOR:
        case USB_CLASS_SPECIFIC_INTERFACE_DESCRIPTOR:
        case USB_CLASS_SPECIFIC_ENDPOINT_DESCRIPTOR:
            // Not implemented
            __asm__ volatile("bkpt");
            break;
    }
}