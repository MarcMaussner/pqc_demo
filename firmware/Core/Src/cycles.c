#include "cycles.h"

void cycles_init(void) {
    /* 1. Enable TRCENA bit in DEMCR (Debug Exception and Monitor Control Register) 
     *    This must be set before accessing DWT registers. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* 2. Unlock DWT access (required on Cortex-M7 like STM32F7) */
    DWT->LAR = 0xC5ACCE55; 
    
    /* 3. Reset the counter */
    DWT->CYCCNT = 0;

    /* 4. Enable the cycle counter bit in DWT control register */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
