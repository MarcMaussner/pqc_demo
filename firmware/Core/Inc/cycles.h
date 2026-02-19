#ifndef __CYCLES_H
#define __CYCLES_H

#include <stdint.h>
#include "stm32f7xx.h"

/**
 * @brief  Initializes the DWT cycle counter.
 *         Enables the TRCENA bit in DEMCR and the CYCCNTENA bit in DWT.
 */
void cycles_init(void);

/**
 * @brief  Resets the cycle counter to zero.
 */
static inline void cycles_reset(void) {
    DWT->CYCCNT = 0;
}

/**
 * @brief  Returns the current 32-bit cycle count.
 * @return Current value of DWT->CYCCNT.
 */
static inline uint32_t cycles_get(void) {
    return DWT->CYCCNT;
}

#endif /* __CYCLES_H */
