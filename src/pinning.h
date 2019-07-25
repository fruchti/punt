#pragma once

// Port A
#define PIN_LIGHT_SENSOR        1   // PA1
#define PIN_TEST_POINT          8   // PA8
#define PIN_UART_TX             9   // PA9  - USART1_TX
#define PIN_UART_RX             10  // PA10 - USART1_RX
#define PIN_USB_DM              11  // PA11 - USB_DM
#define PIN_USB_DP              12  // PA12 - USB_DP
#define PIN_USB_PULLUP          15  // PA15 - 1.5 kΩ to D+

// Port B
#define PIN_LED                 0   // PB0  - Status LED
#define PIN_SELECT              4   // PB4  - Bus select line
#define PIN_SCL                 6   // PB6  - I2C1_SCL
#define PIN_SDA                 7   // PB7  - I2C1_SDA