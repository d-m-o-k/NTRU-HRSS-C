#ifndef POLY_H
#define POLY_H
#include <stdint.h>
#include "param.h"
#include "poly.h"
#include "fips202.h"

void sample_T(poly *res, const uint8_t seed[32]); // retourne un polynôme de {-1, 0, 1}^(N-1), avec le dernier coefficient (pour X^(N-1) à 0)
int8_t corr_sign(const poly *a); // calcule le signe de <a, x*a>
void sample_T_plus(poly *res, const uint8_t seed[32]); // corrige un polynôme dont l'autocorrélation est négative
void poly_sample_hrss(poly *res, const uint8_t seed[32]); // sample un polynôme dans T_+ en utilisant les trois fonctions précédentes


#endif
