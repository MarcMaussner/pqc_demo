#ifndef __CRYPTO_HARNESS_H
#define __CRYPTO_HARNESS_H

#include <stdint.h>

/**
 * @brief  Runs a baseline RSA-2048 benchmark and prints cycles to UART.
 */
void benchmark_rsa(void);

/**
 * @brief  Runs a PQC Dilithium2/Kyber benchmark and prints cycles to UART.
 */
void benchmark_pqc(void);

#endif /* __CRYPTO_HARNESS_H */
