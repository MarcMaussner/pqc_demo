#include "randombytes.h"

/**
 * @brief  Minimal randombytes implementation for PQC benchmarking.
 *         NOTE: This is NOT cryptographically secure!
 */
int PQCLEAN_randombytes(uint8_t *out, size_t outlen) {
    static uint32_t seed = 0x12345678;
    for (size_t i = 0; i < outlen; i++) {
        // Simple LCG
        seed = 1664525u * seed + 1013904223u;
        out[i] = (uint8_t)(seed >> 24);
    }
    return 0;
}
