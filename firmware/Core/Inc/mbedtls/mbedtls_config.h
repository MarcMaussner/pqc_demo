/**
 * \file mbedtls_config.h
 * \brief Minimal configuration for RSA-2048 benchmarking
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_STD_CALLOC calloc
#define MBEDTLS_PLATFORM_STD_FREE free

/* Mbed TLS feature support */
#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_MD_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_CIPHER_C

/* For bignum */
#define MBEDTLS_MPI_MAX_SIZE 512 /**< Maximum number of bytes for usable MPIs. 2048-bit RSA = 256 bytes */

/* For platform util */
#define MBEDTLS_PLATFORM_UTIL_C

/* mbedTLS 3.x requires a few more for logic */
#define MBEDTLS_CONSTANT_TIME_C

/* Ensure no time requirement */
#undef MBEDTLS_HAVE_TIME
#undef MBEDTLS_HAVE_TIME_DATE

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
