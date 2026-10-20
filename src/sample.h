#ifndef SAMPLE_H
#define SAMPLE_H
#include <stdint.h>
#include "param.h"
#include "poly.h"
#include "fips202.h"

void sample_T(poly *res, const uint8_t seed[32]); // retourne un polynôme de {-1, 0, 1}^(N-1), avec le dernier coefficient (pour X^(N-1) à 0)
int8_t corr_sign(const poly *a); // calcule le signe de <a, x*a>
void sample_T_plus(poly *res, const uint8_t seed[32]); // corrige un polynôme dont l'autocorrélation est négative

void sample_nonce_r(poly *res, const uint8_t seed[32]); //génération du nonce r

void sample_KAT_rm(poly *r, poly *m, const uint8_t seed[32]); // génération simultanée de m et r pour la KAT

#endif
