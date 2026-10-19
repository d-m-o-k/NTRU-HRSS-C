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
    poly g;poly fq;poly phi_1;poly three;

    uint8_t out[64];uint8_t seed_f[32];uint8_t seed_g[32];
    uint8_t buf[33];
    buf[0] = 0x00;              // préfixe "clé" (f, g)
    memcpy(buf + 1, seed, 32);
    shake256(out, 64, buf, 33);

    memcpy(seed_f, out, 32);
    memcpy(seed_g, out + 32, 32);

    sample_T_plus(f, seed_f);
    sample_T_plus(&g, seed_g);


    if (!poly_inv_q(&fq, f)){return 0;} // f_q = (1/f) mod q keygen échoue si = 0

    poly_zero(&phi_1); phi_1.coef[0] = -1; phi_1.coef[1] = 1; //phi_1 = X - 1
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
