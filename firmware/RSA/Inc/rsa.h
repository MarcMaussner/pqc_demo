#ifndef __RSA_H
#define __RSA_H

#include "bigint.h"

/**
 * @brief  Performs a raw RSA public key operation (r = m^e mod n).
 * @param  r Result BigInt.
 * @param  m Message BigInt.
 * @param  e Public exponent BigInt (usually 65537).
 * @param  n Modulus BigInt (2048-bit).
 */
void rsa_public_op(BigInt* r, const BigInt* m, const BigInt* e, const BigInt* n);

#endif /* __RSA_H */
