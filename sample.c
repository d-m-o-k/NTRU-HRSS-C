#include <stdio.h>
#include <stdlib.h>
#include "fips202.h"
#include "sample.h"
#include "param.h"
#include "poly.h"
#include "fips202.h"


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
    s = corr_sign(res);

    for(size_t i = 0; i < (n-1)/2; i++){
        res->coef[2i] = s*res->coef[2i];
    }
}
