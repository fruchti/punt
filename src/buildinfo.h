#pragma once

#include<stdint.h>

extern char __BUILD_DATE;
extern char __BUILD_NUMBER;

#define BUILD_DATE      ((uint32_t)&__BUILD_DATE)
#define BUILD_NUMBER    ((uint32_t)&__BUILD_NUMBER)

typedef struct
{
    uint32_t build_date;
    uint32_t build_number;
} BuildInfo_t;

extern const BuildInfo_t BuildInfo;
