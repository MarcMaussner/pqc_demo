#include "crypto_harness.h"
#include "cycles.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <string.h>

/* Crypto includes */
#include "rsa.h"
#include "bigint.h"
#include "api.h" // Note: Inclusion order/names might need care for PQC

// PQC Prototypes (Explicitly to avoid include conflicts if any)
#include "deps/PQClean/crypto_sign/ml-dsa-44/clean/api.h"
#include "deps/PQClean/crypto_kem/ml-kem-512/clean/api.h"

extern UART_HandleTypeDef huart1;

/* Static buffers to avoid stack overflow */
static uint8_t pk[5000];
static uint8_t sk[5000];
static uint8_t sig[5000];
static uint8_t ct[2048];
static uint8_t ss1[128];
static uint8_t ss2[128];
static uint8_t msg_buf[256];

void benchmark_rsa(void) {
    uint32_t start, end;
    BigInt n, e, m, r;
    char buf[128];
    
    sprintf(buf, "\r\n--- RSA-2048 (Classical Baseline) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    bi_set_uint32(&n, 0xABCDEF01);
    bi_set_uint32(&e, 65537);
    bi_set_uint32(&m, 0x12345678);

    cycles_reset();
    start = cycles_get();
    rsa_public_op(&r, &m, &e, &n);
    end = cycles_get();

    sprintf(buf, "UART >> RSA: Public Op took %lu cycles\r\n", end - start);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
}

void benchmark_pqc(void) {
    uint32_t start, end;
    char buf[128];
    size_t siglen;

    /* --- ML-DSA-44 (Dilithium2) --- */
    sprintf(buf, "\r\n--- PQC: ML-DSA-44 (Dilithium2) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLDSA44_CLEAN_crypto_sign_keypair(pk, sk);
    end = cycles_get();
    sprintf(buf, "UART >> ML-DSA-44: Keygen took %lu cycles\r\n", end - start);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLDSA44_CLEAN_crypto_sign_signature(sig, &siglen, (uint8_t*)"test", 4, sk);
    end = cycles_get();
    sprintf(buf, "UART >> ML-DSA-44: Sign took %lu cycles\r\n", end - start);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLDSA44_CLEAN_crypto_sign_verify(sig, siglen, (uint8_t*)"test", 4, pk);
    end = cycles_get();
    sprintf(buf, "UART >> ML-DSA-44: Verify took %lu cycles\r\n", end - start);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* --- ML-KEM-512 (Kyber512) --- */
    sprintf(buf, "\r\n--- PQC: ML-KEM-512 (Kyber512) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLKEM512_CLEAN_crypto_kem_keypair(pk, sk);
    end = cycles_get();
    sprintf(buf, "UART >> ML-KEM-512: Keygen took %lu cycles\r\n", end - start);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLKEM512_CLEAN_crypto_kem_enc(ct, ss1, pk);
    end = cycles_get();
    sprintf(buf, "UART >> ML-KEM-512: Encaps took %lu cycles\r\n", end - start);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLKEM512_CLEAN_crypto_kem_dec(ss2, ct, sk);
    end = cycles_get();
    sprintf(buf, "UART >> ML-KEM-512: Decaps took %lu cycles\r\n", end - start);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
}
