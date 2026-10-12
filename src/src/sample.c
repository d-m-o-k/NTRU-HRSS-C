#include <stdio.h>
#include <stdlib.h>
#include "fips202.h"
#include "sample.h"
#include "param.h"
#include "poly.h"
#include "fips202.h"

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

int keygen_hrss(poly *f, poly *h, const uint8_t seed[32]){
    poly g;poly fq;poly phi_1;

    uint8_t out[64];uint8_t seed_f[32];uint8_t seed_g[32];
    shake256(out, 64, seed, 32);
    memcpy(seed_f, out, 32);
    memcpy(seed_g, out + 32, 32);

    sample_T_plus(&g, seed_f);
    sample_T_plus(f, seed_g);

    if (!poly_inv_q(&fq, f)){return 0;} // f_q = (1/f) mod q keygen échoue si = 0

    poly_zero(&phi_1); phi_1.coef[0] = -1; phi_1.coef[1] = 1; //phi_1 = X - 1


    poly_mul_q(h, &g, &phi_1);
    poly_mul_q(h, h, &fq);

    return 1;
}
