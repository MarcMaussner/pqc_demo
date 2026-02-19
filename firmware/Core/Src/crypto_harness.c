#include "crypto_harness.h"
#include "cycles.h"
#include "stack_watermark.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <string.h>

/* Crypto includes */
#include "rsa.h"
#include "bigint.h"

// PQC Prototypes
#include "deps/PQClean/crypto_sign/ml-dsa-44/clean/api.h"
#include "deps/PQClean/crypto_kem/ml-kem-512/clean/api.h"
#include "deps/PQClean/crypto_sign/falcon-512/clean/api.h"
#include "deps/PQClean/crypto_sign/sphincs-sha2-128s-simple/clean/api.h"

extern UART_HandleTypeDef huart1;

/* Static buffers to avoid stack overflow for very large items.
   SPHINCS+ signature is ~7.8KB, so we need larger buffers. */
static uint8_t pk[8192];
static uint8_t sk[8192];
static uint8_t sig[8192];
static uint8_t ct[2048];
static uint8_t ss1[128];
static uint8_t ss2[128];

void benchmark_rsa(void) {
    uint32_t start, end;
    size_t stack_used;
    BigInt n, e, m, r;
    char buf[128];
    
    sprintf(buf, "\r\n--- RSA-2048 (Classical Baseline) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    bi_set_uint32(&n, 0xABCDEF01);
    bi_set_uint32(&e, 65537);
    bi_set_uint32(&m, 0x12345678);

    sprintf(buf, "UART >> RSA: Starting Public Op...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    rsa_public_op(&r, &m, &e, &n);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();

    sprintf(buf, "UART >> RSA: Public Op took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
}

void benchmark_pqc(void) {
    uint32_t start, end;
    size_t stack_used;
    char buf[128];
    size_t siglen;

    /* --- ML-DSA-44 (Dilithium2) --- */
    sprintf(buf, "\r\n--- PQC: ML-DSA-44 (Dilithium2) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-DSA-44: Starting Keygen...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLDSA44_CLEAN_crypto_sign_keypair(pk, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-DSA-44: Keygen took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-DSA-44: Starting Sign...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLDSA44_CLEAN_crypto_sign_signature(sig, &siglen, (uint8_t*)"test", 4, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-DSA-44: Sign took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* --- ML-KEM-512 (Kyber512) --- */
    sprintf(buf, "\r\n--- PQC: ML-KEM-512 (Kyber512) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-KEM-512: Starting Keygen...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLKEM512_CLEAN_crypto_kem_keypair(pk, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-KEM-512: Keygen took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-KEM-512: Starting Encaps...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_MLKEM512_CLEAN_crypto_kem_enc(ct, ss1, pk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-KEM-512: Encaps took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* --- Falcon-512 --- */
    sprintf(buf, "\r\n--- PQC: Falcon-512 ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> Falcon-512: Starting Keygen...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_FALCON512_CLEAN_crypto_sign_keypair(pk, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> Falcon-512: Keygen took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> Falcon-512: Starting Sign...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_FALCON512_CLEAN_crypto_sign_signature(sig, &siglen, (uint8_t*)"test", 4, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> Falcon-512: Sign took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* --- SPHINCS+-sha2-128s-simple --- */
    sprintf(buf, "\r\n--- PQC: SPHINCS+ (SHA2-128s) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> SPHINCS+: Starting Keygen...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_SPHINCSSHA2128SSIMPLE_CLEAN_crypto_sign_keypair(pk, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> SPHINCS+: Keygen took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> SPHINCS+: Starting Sign...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    PQCLEAN_SPHINCSSHA2128SSIMPLE_CLEAN_crypto_sign_signature(sig, &siglen, (uint8_t*)"test", 4, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> SPHINCS+: Sign took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
}
