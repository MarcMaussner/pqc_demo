#include "rsa.h"

void rsa_public_op(BigInt* r, const BigInt* m, const BigInt* e, const BigInt* n) {
    bi_mod_exp(r, m, e, n);
}
