#include "crypto_harness.h"
#include "cycles.h"
#include "stack_watermark.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <string.h>

/* Crypto includes */
#include "mbedtls/rsa.h"
#include "mbedtls/platform.h"

// PQC Prototypes
#include "deps/PQClean/crypto_sign/ml-dsa-44/clean/api.h"
#include "deps/PQClean/crypto_kem/ml-kem-512/clean/api.h"
#include "deps/PQClean/crypto_sign/falcon-512/clean/api.h"
#include "deps/PQClean/crypto_sign/sphincs-sha2-128s-simple/clean/api.h"

extern UART_HandleTypeDef huart1;

/* Static buffers to avoid stack overflow for very large items. */
static uint8_t pk[8192];
static uint8_t sk[8192];
static uint8_t sig[8192];
static uint8_t ct[2048];
static uint8_t ss1[128];
static uint8_t ss2[128];

void benchmark_rsa(void) {
    uint32_t start, end;
    size_t stack_used;
    char buf[128];
    mbedtls_rsa_context rsa;
    unsigned char input[256];
    unsigned char output[256];
    
    sprintf(buf, "\r\n--- RSA-2048 (mbedTLS Baseline) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    mbedtls_rsa_init(&rsa);
    
    /* Set padding mode using public API */
    mbedtls_rsa_set_padding(&rsa, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);

    /* Setup a dummy but valid size 2048-bit key */
    mbedtls_mpi N, E;
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&E);
    mbedtls_mpi_lset(&E, 65537);
    mbedtls_mpi_lset(&N, 1);
    mbedtls_mpi_shift_l(&N, 2047); // Rough approximation of a 2048-bit N

    mbedtls_rsa_import(&rsa, &N, NULL, NULL, NULL, &E);
    mbedtls_rsa_complete(&rsa);

    memset(input, 0xAA, sizeof(input));
    input[0] = 0; // Ensure input < N

    sprintf(buf, "UART >> RSA: Starting Public Op (mbedTLS)...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    mbedtls_rsa_public(&rsa, input, output);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();

    sprintf(buf, "UART >> RSA: Public Op took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    mbedtls_rsa_free(&rsa);
    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&E);
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
