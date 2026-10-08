#ifndef POLY_H
#define POLY_H
#include <stdint.h>
#include "param.h"

typedef struct {
    int16_t coef[N]; // N coefficients dans les corps de l'algorithme, on choisit int16_t car les coefficients des polynômes secrets sont à coefficients ternaires donc int16_t suffit
} poly;
// on considère pour toutes les opérations que les ponyômes sont à coefficients dans [−q/2, q/2 - 1]


void poly_zero(poly *a); // (ré)initialise un polynôme comme polynôme nul
void poly_copy(poly *res, const poly *a); // copie a dans res

void poly_add_3(poly *res, const poly *a, const poly *b); // res = a + b mod 3, phi_N
void poly_add_q(poly *res, const poly *a, const poly *b); // res = a + b mod q, phi_N
void poly_add_2(poly *res, const poly *a, const poly *b); // res = a + b mod q, phi_N

void poly_neg_3(poly *res, const poly *a); // fonctions de calcul d'opposés pour la soustraction
void poly_neg_q(poly *res, const poly *a);

void poly_sub_3(poly *res, const poly *a, const poly *b); // res = a - b mod 3, phi_N
void poly_sub_q(poly *res, const poly *a, const poly *b);

void poly_mul_3(poly *res, const poly *a, const poly *b); // res = a * b mod 3, phi_N
void poly_mul_q(poly *res, const poly *a, const poly *b);
void poly_mul_2(poly *res, const poly *a, const poly *b);


void poly_red_3(poly *res, const poly *a); // projection mod 3
void poly_red_2(poly *res, const poly *a); // projection mod 2

int poly_inv_3(poly *res, const poly *a); // int pour retourner un statut dans le cas de non-inversibilité
int poly_inv_2(poly *res, const poly *a); // pour la première étape du lifting d'Hensel pour le calcul d'inverse modulo q
int poly_inv_q(poly *res, const poly *a);

void poly_sample(poly *res, const long long seed); // retourne un polynôme de {-1, 0, 1}^(N-1), avec le dernier coefficient (pour X^(N-1) à 0)
int8_t poly_corr_sign(const poly *a); // calcule le signe de <a, x*a>
void poly_correct_sign(poly *a); // corrige un polynôme dont l'autocorrélation est négative
void poly_sample_hrss(poly *res, const long long seed); // sample un polynôme dans T_+ en utilisant les trois fonctions précédentes
//utilisation temporaire d'une seed pour la génération aléatoire en attendant une implémentation finale de randombytes via fips202.c (sample128)

#endif
