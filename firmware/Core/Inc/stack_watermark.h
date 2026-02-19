#ifndef STACK_WATERMARK_H
#define STACK_WATERMARK_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief  Initializes stack watermarking by filling the stack with a pattern.
 *         Should be called as early as possible after stack initialization.
 */
void stack_watermark_init(void);

/**
 * @brief  Calculates the peak stack usage since the last initialization.
 * @return Number of bytes used on the stack.
 */
size_t stack_watermark_get_usage(void);

#endif /* STACK_WATERMARK_H */
