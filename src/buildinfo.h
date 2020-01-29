#pragma once

#include <stdint.h>

#define BUILD_VERSION_MAJOR     0
#define BUILD_VERSION_MINOR     1
#define BUILD_VERSION_PATCH     0

#define BUILD_DATE              ((uint32_t)&__BUILD_DATE)
#define BUILD_NUMBER            ((uint32_t)&__BUILD_NUMBER)

extern char __BUILD_DATE;
extern char __BUILD_NUMBER;