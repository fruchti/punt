#pragma once

#define _PASTE(x, y)            x ## y
#define PASTE(x, y)             _PASTE(x, y)

#define MODULE_OWNS_PERIPHERAL(peripheral) \
                                void *_PERIPHERAL_OWNERSHIP_ ## peripheral \
                                = (void*)(peripheral)

#define MODULE_OWNS_PIN(gpio, pin) \
                                void *PASTE(_PIN_OWNERSHIP_ ## gpio ## _, pin) \
                                = (void*)(gpio + pin)
