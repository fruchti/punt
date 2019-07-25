#pragma once

#include <stdint.h>
#include <stddef.h>
#include "stm32f103x6.h"

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t bcdUSB;
        uint8_t bDeviceClass;
        uint8_t bDeviceSubClass;
        uint8_t bDeviceProtocol;
        uint8_t bMaxPacketSize0;
        uint16_t idVendor;
        uint16_t idProduct;
        uint16_t bcdDevice;
        uint8_t iManufacturer;
        uint8_t iProduct;
        uint8_t iSerialNumber;
        uint8_t bNumConfigurations;
    };
    uint8_t raw[18];
} __attribute__((packed)) USB_DeviceDescriptor_t;

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wTotalLength;
        uint8_t bNumInterfaces;
        uint8_t bConfigurationValue;
        uint8_t iConfiguration;
        uint8_t bmAttributes;
        uint8_t bMaxPower;        
    } __attribute__((packed, aligned(1)));
    uint8_t raw[9];
} __attribute__((packed, aligned(1))) USB_ConfigurationDescriptor_t;

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bInterfaceNumber;
        uint8_t bAlternateSetting;
        uint8_t bNumEndpoints;
        uint8_t bInterfaceClass;
        uint8_t bInterfaceSubClass;
        uint8_t bInterfaceProtocol;
        uint8_t iInterface;        
    } __attribute__((packed, aligned(1)));
    uint8_t raw[9];
} __attribute__((packed, aligned(1))) USB_InterfaceDescriptor_t;

// Endpoint direction for the bEndpointAddress field
#define USB_ENDPOINT_OUT                        0x00
#define USB_ENDPOINT_IN                         0x80

// Flags in bmAttributes
#define USB_ENDPOINT_CONTROL                    0x00
#define USB_ENDPOINT_ISOCHRONOUS                0x01
#define USB_ENDPOINT_BULK                       0x02
#define USB_ENDPOINT_INTERRUPT                  0x03

#define USB_ENDPOINT_NO_SYNCHRONIZATION         0x00
#define USB_ENDPOINT_ASYNCHRONOUS               0x04
#define USB_ENDPOINT_ADAPTIVE                   0x08
#define USB_ENDPOINT_SYNCHRONOUS                0x0c

#define USB_ENDPOINT_DATA                       0x00
#define USB_ENDPOINT_FEEDBACK                   0x10
#define USB_ENDPOINT_IMPLICIT_FEEDBACK_DATA     0x20

typedef union
{
    struct
    {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bEndpointAddress;
        uint8_t bmAttributes;
        uint16_t wMaxPacketSize;
        uint8_t bInterval;        
    } __attribute__((packed, aligned(1)));
    uint8_t raw[7];
} __attribute__((packed, aligned(1))) USB_EndpointDescriptor_t;

typedef union
{
    struct
    {
        USB_ConfigurationDescriptor_t configuration;
        USB_InterfaceDescriptor_t main_interface;
        USB_EndpointDescriptor_t data_in_endpoint;
        USB_EndpointDescriptor_t data_out_endpoint;
    } __attribute__((packed, aligned(1)));
    uint8_t raw[62];
} __attribute__((packed, aligned(1))) USB_WholeDescriptor_t;


typedef enum
{
    USB_DEVICE_DESCRIPTOR = 0x01,
    USB_CONFIGURATION_DESCRIPTOR = 0x02,
    USB_STRING_DESCRIPTOR = 0x03,
    USB_INTERFACE_DESCRIPTOR = 0x04,
    USB_ENDPOINT_DESCRIPTOR = 0x05,
    USB_DEVICE_QUALIFIER_DESCRIPTOR = 0x06,
    USB_OTHER_DESCRIPTOR = 0x07,
    USB_INTERFACE_POWER_DESCRIPTOR = 0x08,
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR = 0x0b,
    USB_CLASS_SPECIFIC_INTERFACE_DESCRIPTOR = 0x24,
    USB_CLASS_SPECIFIC_ENDPOINT_DESCRIPTOR = 0x25

} __attribute__((packed)) USB_DescriptorType_t;

#define USB_STRING_DESCRIPTOR_LENGTH(...)       \
    (sizeof((uint16_t[]){__VA_ARGS__}) + 2)
#define USB_BUILD_STRING_DESCRIPTOR(...)        \
    {USB_STRING_DESCRIPTOR_LENGTH(__VA_ARGS__)  \
    | (USB_STRING_DESCRIPTOR << 8), __VA_ARGS__}


void USB_HandleGetDescriptor(USB_DescriptorType_t descriptor_type,
    int descriptor_index, const uint8_t **reply_data, int *reply_length,
    uint8_t *reply_response);
