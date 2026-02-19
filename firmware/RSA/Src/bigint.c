#include "bigint.h"
#include <string.h>

void bi_from_bytes(BigInt* a, const uint8_t* buf, int len) {
    memset(a->words, 0, sizeof(a->words));
    int words = (len + 3) / 4;
    for (int i = 0; i < len; i++) {
        int word_idx = i / 4;
        int byte_idx = i % 4;
        a->words[word_idx] |= (uint32_t)buf[len - 1 - i] << (byte_idx * 8);
    }
}

void bi_set_uint32(BigInt* a, uint32_t val) {
    memset(a->words, 0, sizeof(a->words));
    a->words[0] = val;
}

int bi_compare(const BigInt* a, const BigInt* b) {
    for (int i = BI_MAX_WORDS - 1; i >= 0; i--) {
        if (a->words[i] > b->words[i]) return 1;
        if (a->words[i] < b->words[i]) return -1;
    }
    return 0;
}

void bi_copy(BigInt* dst, const BigInt* src) {
    memcpy(dst->words, src->words, sizeof(dst->words));
}

// Simple schoolbook addition for modular reduction
static void bi_add_internal(BigInt* r, const BigInt* a, const BigInt* b) {
    uint64_t carry = 0;
    for (int i = 0; i < BI_MAX_WORDS; i++) {
        uint64_t sum = (uint64_t)a->words[i] + b->words[i] + carry;
        r->words[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
}

// Simple subtraction
static int bi_sub_internal(BigInt* r, const BigInt* a, const BigInt* b) {
    int64_t borrow = 0;
    for (int i = 0; i < BI_MAX_WORDS; i++) {
        int64_t diff = (int64_t)a->words[i] - b->words[i] - borrow;
        r->words[i] = (uint32_t)diff;
        borrow = (diff < 0) ? 1 : 0;
    }
    return (int)borrow;
}

void bi_mod_mul(BigInt* r, const BigInt* a, const BigInt* b, const BigInt* m) {
    // This is a naive implementation for the baseline benchmarking.
    // In a real crypto lib, Montgomery multiplication would be used here.
    // For M1 baseline, we simulate the workload.
    BigInt tmp;
    bi_set_uint32(&tmp, 0);
    for (int i = BI_MAX_WORDS * 32 - 1; i >= 0; i--) {
        bi_add_internal(&tmp, &tmp, &tmp); // Double
        if ((b->words[i / 32] >> (i % 32)) & 1) {
            bi_add_internal(&tmp, &tmp, a);
        }
        while (bi_compare(&tmp, m) >= 0) {
            bi_sub_internal(&tmp, &tmp, m);
        }
    }
    bi_copy(r, &tmp);
}

void bi_mod_exp(BigInt* r, const BigInt* b, const BigInt* e, const BigInt* m) {
    BigInt res;
    bi_set_uint32(&res, 1);
    
    for (int i = BI_MAX_WORDS * 32 - 1; i >= 0; i--) {
        bi_mod_mul(&res, &res, &res, m); // Square
        if ((e->words[i / 32] >> (i % 32)) & 1) {
            bi_mod_mul(&res, &res, b, m); // Multiply
        }
    }
    bi_copy(r, &res);
}
