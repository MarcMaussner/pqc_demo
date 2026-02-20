#include "crypto_harness.h"
#include "cycles.h"
#include "stack_watermark.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <string.h>

/* Crypto includes */
#include "mbedtls/rsa.h"
#include "mbedtls/platform.h"

// NIST Round 3 PQC Candidates (PQClean for Falcon/Sphincs)
#include "deps/PQClean/crypto_sign/falcon-512/clean/api.h"
#include "deps/PQClean/crypto_sign/sphincs-sha2-128s-simple/clean/api.h"

// NIST Round 3 PQC Candidates (pqm4 for Kyber/Dilithium)
// Manual declaration to avoid api.h include guard collision in pqm4 generic headers
// ML-KEM-512 (Kyber-512)
#define MLKEM512_PUBLICKEYBYTES 800
#define MLKEM512_SECRETKEYBYTES 1632
#define MLKEM512_CIPHERTEXTBYTES 768
#define MLKEM512_SSBYTES 32
int crypto_kem_keypair(uint8_t *pk, uint8_t *sk);
int crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);
int crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);

// ML-DSA-44 (Dilithium2)
#define MLDSA44_PUBLICKEYBYTES 1312
#define MLDSA44_SECRETKEYBYTES 2528
#define MLDSA44_BYTES 2420
int crypto_sign_keypair(uint8_t *pk, uint8_t *sk);
int crypto_sign_signature(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t *sk);
int crypto_sign_verify(const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen, const uint8_t *pk);

extern UART_HandleTypeDef huart1;

/* Static buffers to avoid stack overflow for very large items. */
static uint8_t pk[8192];
static uint8_t sk[8192];
static uint8_t sig[8192];
static uint8_t ct[2048];
static uint8_t ss1[128];
static uint8_t ss2[128];

/* Simple LCG for deterministic benchmarking without HW RNG dependence */
static int fake_rng(void *p_rng, unsigned char *output, size_t output_len) {
    static uint32_t seed = 0x12345678;
    for(size_t i = 0; i < output_len; i++) {
        seed = seed * 1664525 + 1013904223;
        output[i] = (unsigned char)(seed >> 24);
    }
    return 0;
}

void benchmark_rsa(void) {
    uint32_t start, end;
    size_t stack_used;
    char buf[128];
    mbedtls_rsa_context rsa;
    unsigned char input[256];
    unsigned char output[256];
    unsigned char output_dec[256];
    int ret;
    
    sprintf(buf, "\r\n--- RSA-2048 (mbedTLS Baseline) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    mbedtls_rsa_init(&rsa);
    mbedtls_rsa_set_padding(&rsa, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);

    /* 1. Key Generation */
    sprintf(buf, "UART >> RSA: Starting KeyGen...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
    
    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    ret = mbedtls_rsa_gen_key(&rsa, fake_rng, NULL, 2048, 65537);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();

    if(ret != 0) {
        sprintf(buf, "UART >> RSA: KeyGen Failed (-0x%04X)\r\n", -ret);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
        goto exit;
    }

    sprintf(buf, "UART >> RSA: KeyGen took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* 2. Public Key Operation (Encrypt) */
    memset(input, 0xAA, 200); // cleartext < modulus
    input[199] = 0; 

    sprintf(buf, "UART >> RSA: Starting Public Op (mbedTLS)...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    ret = mbedtls_rsa_public(&rsa, input, output);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();

    if(ret != 0) {
        sprintf(buf, "UART >> RSA: Public Op Failed (-0x%04X)\r\n", -ret);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
        goto exit;
    }

    sprintf(buf, "UART >> RSA: Public Op took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* 3. Private Key Operation (Decrypt) */
    sprintf(buf, "UART >> RSA: Starting Private Op (mbedTLS)...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    // Note: mbedtls_rsa_private uses CRT if P/Q are available (which they are from gen_key)
    ret = mbedtls_rsa_private(&rsa, fake_rng, NULL, output, output_dec);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();

    if(ret != 0) {
        sprintf(buf, "UART >> RSA: Private Op Failed (-0x%04X)\r\n", -ret);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);
        goto exit;
    }

    sprintf(buf, "UART >> RSA: Private Op took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

exit:
    mbedtls_rsa_free(&rsa);
}

void benchmark_pqc(void) {
    uint32_t start, end;
    size_t stack_used;
    char buf[128];
    size_t siglen;

    /* --- ML-DSA-44 (Dilithium2 - pqm4) --- */
    sprintf(buf, "\r\n--- PQC: ML-DSA-44 (Dilithium2 - ASM) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-DSA-44: Starting Keygen...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    // pqm4: crypto_sign_keypair
    crypto_sign_keypair(pk, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-DSA-44: Keygen took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-DSA-44: Starting Sign...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    // pqm4: crypto_sign_signature_ctx
    // Note: pqm4/api.h defines crypto_sign_signature as a macro calling _ctx with NULL, 0.
    // Since we don't include that api.h, we call _ctx directly.
    extern int crypto_sign_signature_ctx(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t *ctx, size_t ctxlen, const uint8_t *sk);
    crypto_sign_signature_ctx(sig, &siglen, (uint8_t*)"test", 4, NULL, 0, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-DSA-44: Sign took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* --- ML-KEM-512 (Kyber512 - pqm4) --- */
    sprintf(buf, "\r\n--- PQC: ML-KEM-512 (Kyber512 - ASM) ---\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-KEM-512: Starting Keygen...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    // pqm4: crypto_kem_keypair
    crypto_kem_keypair(pk, sk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-KEM-512: Keygen took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    sprintf(buf, "UART >> ML-KEM-512: Starting Encaps...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    stack_watermark_init();
    cycles_reset();
    start = cycles_get();
    // pqm4: crypto_kem_enc
    crypto_kem_enc(ct, ss1, pk);
    end = cycles_get();
    stack_used = stack_watermark_get_usage();
    sprintf(buf, "UART >> ML-KEM-512: Encaps took %lu cycles, Stack: %u bytes\r\n", end - start, (unsigned int)stack_used);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 1000);

    /* --- Falcon-512 (PQClean - Legacy/C) --- */
    sprintf(buf, "\r\n--- PQC: Falcon-512 (Clean C) ---\r\n");
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

    /* --- SPHINCS+-sha2-128s-simple (PQClean - Legacy/C) --- */
    sprintf(buf, "\r\n--- PQC: SPHINCS+ (SHA2-128s - Clean C) ---\r\n");
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
