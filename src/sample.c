#include <stdio.h>
#include <stdlib.h>
#include "fips202.h"
#include "sample.h"
#include "param.h"
#include "poly.h"
#include "fips202.h"

static inline int16_t centering_3(int64_t a){
    a %= 3; // pour le moment la fonction suppose que p sera toujours 3
    a -= 3 * (a == 2);
    a += 3 * (a == -2);
    return a;
}

static inline int16_t centering_q(int64_t a){
    a &= (q - 1); // masque pour le modulo car q est nécessairement une puissance de 2
    a -= q * (a > (q/2 - 1));
    return a;
}


// pour le KAT : flux de bit non continu à vérifier plus tard

void sample_T(poly *res, const uint8_t seed[32]){
    uint8_t ubytes[N - 1];
    shake256(ubytes, sizeof(ubytes), seed, 32);

    poly_zero(res);
    for(size_t i = 0; i < N - 1; i++){
        int16_t x = (((ubytes[i])&1) + ((ubytes[i]>>1)&1) - ((ubytes[i]>>2)&1) - ((ubytes[i]>>3)&1)); //pour le moment on ne prend pas le flux continu des bits pour simplifier donc non-KAT!
        res->coef[i] = centering_3(x);
    }
}

int8_t corr_sign(const poly *a){
    int16_t s = 0;

    for(size_t i = 0; i < N - 2; i++){
        s += a->coef[i]*a->coef[i+1]; // car a son dernier coefficient nul et x*a son premier
    }

    return (int8_t)((s > 0) - (s < 0));
}

void sample_T_plus(poly *res, const uint8_t seed[32]){
    sample_T(res, seed);
    int8_t s = corr_sign(res);

    for(size_t i = 0; i < (N-1)/2; i++){
        int16_t c = res->coef[2*i];
        res->coef[2*i] = (c & ~(-(s < 0))) | ((-c) & (-(s < 0)));

    }
}


void sample_nonce_r(poly *res, const uint8_t seed[32]){
    int16_t tab[700] = {0};

    for(size_t i = 0; i < 233; i++){
        tab[i] = 1;
        tab[i + 233] = -1;
    }

    uint8_t ubytes[4096];
    shake256(ubytes, sizeof(ubytes), seed, 32);
    size_t bit_idx = 0;

    uint16_t j = 0;int16_t temp = 0; //fisher yates partiel pour générer un polynôme équilibré aléatoire
    for(size_t i = N - 2; i >= N - 2 - 2*233 + 1; i--){

        size_t bits = 1;
        for(size_t t = i; t > 1; t >>= 1) bits++;

        // tirage uniforme par rejet
        do {
            j = 0;
            for(size_t k = 0; k < bits; k++){
                size_t b = bit_idx >> 3;
                size_t pi = bit_idx & 7;
                j |= (uint16_t)((ubytes[b] >> pi) & 1) << k;
                bit_idx++;
            }
        } while(j > i);

        temp = tab [i];
        tab[i] = tab[j];
        tab[j] = temp;
    }

    for(size_t i = 0; i < 700; i++){
        res->coef[i] = tab[i];
    }
    res->coef[N - 1] = 0;
}
