#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fips202.h"
#include "sample.h"
#include "param.h"
#include "poly.h"
#include "fips202.h"
#include "coding.h"
#include "sample.h"

int ntru_hrss_keygen(poly *f, poly *h, const uint8_t seed[32]){
    poly g; poly fq; poly phi_1; poly three;

    uint8_t uniformbytes[2 * NTRU_SAMPLE_IID_BYTES]; // 1408 octets

    shake256(uniformbytes, sizeof(uniformbytes), seed, 32);

    sample_T(f, uniformbytes);
    sample_T(&g, uniformbytes + NTRU_SAMPLE_IID_BYTES);

    if (!poly_inv_q(&fq, f)){return 0;}

    poly_zero(&phi_1); phi_1.coef[0] = -1; phi_1.coef[1] = 1;
    poly_zero(&three); three.coef[0] = 3;

    poly_mul_q(h, &g, &phi_1);
    poly_mul_q(h, h, &fq);
    poly_mul_q(h, h, &three);

    return 1;
}

void ntru_hrss_encrypt(poly *c, const poly *m, const  poly *h, const uint8_t seed[32]){
    poly r;
    sample_T(&r, seed);

    poly_mul_q(c, h, &r);
    poly_add_q(c, c, m); // c = h*r + m [q]
}

int ntru_hrss_decrypt(poly *m, const poly *c, const poly *f){
    poly a;poly fp;

    poly_mul_q(&a, c, f);

    for (size_t i = 0; i < N; i++) {
        int16_t v = a.coef[i];
        if (v < -q/4 || v >= q/4){return 0;}
    }

    poly_red_3(&a, &a);

    poly_red_3(&fp, f);
    if (!poly_inv_3(&fp, &fp)){return 0;}

    poly_mul_3(m, &a, &fp);

    return 1;
}
