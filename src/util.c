#include "util.h"

void Util_Delay(unsigned int delay)
{
    SysTick->LOAD = delay;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while(!((SysTick->CTRL) & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL = 0;
}
