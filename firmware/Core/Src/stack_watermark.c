#include "stack_watermark.h"
#include <stdint.h>
#include <stddef.h>

extern char _estack;          // Top of stack (highest address)
extern char _Min_Stack_Size;  // Size of stack from linker

/**
 * @brief  Fill the stack area with a magic pattern (0xDEADBEEF).
 */
void stack_watermark_init(void) {
    uintptr_t estack_addr = (uintptr_t)&_estack;
    uintptr_t stack_len = (uintptr_t)&_Min_Stack_Size;
    uint32_t *stack_start = (uint32_t *)(estack_addr - stack_len);
    uint32_t *sp;
    
    __asm volatile ("mov %0, sp" : "=r" (sp));

    uint32_t *curr = stack_start;
    // Fill up to SP minus 512 words (2KB) safety margin
    while (curr < sp - 512) {
        *curr = 0xDEADBEEF;
        curr++;
    }
}

/**
 * @brief  Find the lowest address where the pattern was overwritten.
 */
size_t stack_watermark_get_usage(void) {
    uintptr_t estack_addr = (uintptr_t)&_estack;
    uintptr_t stack_len = (uintptr_t)&_Min_Stack_Size;
    uint32_t *stack_start = (uint32_t *)(estack_addr - stack_len);
    uint32_t *curr = stack_start;
    
    while (curr < (uint32_t *)estack_addr) {
        if (*curr != 0xDEADBEEF) {
            break;
        }
        curr++;
    }
    
    uintptr_t used_bottom = (uintptr_t)curr;
    if (used_bottom > estack_addr) used_bottom = estack_addr;
    
    return (size_t)(estack_addr - used_bottom);
}
