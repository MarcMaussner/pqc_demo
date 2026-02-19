#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System support */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C

/* Feature support */
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_MD_C
#define MBEDTLS_SHA256_C

/* Minimal entropy for PoC (Not for production!) */
#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#define MBEDTLS_TEST_NULL_ENTROPY

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
