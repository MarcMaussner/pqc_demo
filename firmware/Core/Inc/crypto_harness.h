#ifndef __CRYPTO_HARNESS_H
#define __CRYPTO_HARNESS_H

#include <stdint.h>

/**
 * @brief  Runs a scalable RSA benchmark (2048, 3072, 4096) and prints cycles to UART.
 */
void benchmark_rsa_suite(void);

/**
 * @brief  Runs a PQC Dilithium2/Kyber benchmark and prints cycles to UART.
 */
void benchmark_pqc(void);

#endif /* __CRYPTO_HARNESS_H */
