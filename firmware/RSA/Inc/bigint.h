#ifndef __BIGINT_H
#define __BIGINT_H

#include <stdint.h>
#include <stdbool.h>

#define BI_MAX_WORDS 64 // 64 * 32 = 2048 bits

typedef struct {
    uint32_t words[BI_MAX_WORDS];
} BigInt;

void bi_from_bytes(BigInt* a, const uint8_t* buf, int len);
void bi_to_bytes(const BigInt* a, uint8_t* buf, int len);
void bi_set_uint32(BigInt* a, uint32_t val);

void bi_copy(BigInt* dst, const BigInt* src);
int bi_compare(const BigInt* a, const BigInt* b);

void bi_add(BigInt* r, const BigInt* a, const BigInt* b);
void bi_sub(BigInt* r, const BigInt* a, const BigInt* b);
void bi_mul(BigInt* r, const BigInt* a, const BigInt* b); // Simple schoolbook
void bi_mod_mul(BigInt* r, const BigInt* a, const BigInt* b, const BigInt* m);
void bi_mod_exp(BigInt* r, const BigInt* b, const BigInt* e, const BigInt* m);

#endif /* __BIGINT_H */
