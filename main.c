#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "poly.h"
#include "coding.h"
#include "sample.h"
#include "ntru.h"
#include "param.h"

/* Project for a minimally dependent, fully functional and secure NTRU-HRSS implementation in C */

//pour le moment, main.c sert pour les tests, en l'occurence les tests pour poly.c

// fonction qui print les coefficient non-nuls sou la forme a_i = [ i:a_i ]
static void print_poly(const char *name, const poly *ptr) {
    printf("%s = [", name);
    for (size_t i = 0; i < N; i++) {
        if (ptr->coef[i] != 0) printf(" %zu:%d", i, ptr->coef[i]);
    }
    printf(" ]\n");
}

static int8_t poly_cmp_q(const poly *a, const poly *b){
    int16_t c = a->coef[0] - b->coef[0];   // constante candidate
    for(size_t i = 1; i < N; i++){
        if ((int16_t)(a->coef[i] - b->coef[i]) != c) return 0;
    }
    return 1;
}

static int8_t poly_cmp_3(const poly *a, const poly *b){
    int16_t c = (a->coef[0] - b->coef[0]) % 3;
    if (c < 0) c += 3;
    for(size_t i = 1; i < N; i++){
        int16_t d = (a->coef[i] - b->coef[i]) % 3;
        if (d < 0) d += 3;
        if (d != c) return 0;
    }
    return 1;
}

static int8_t poly_cmp(const poly *a, const poly *b) {
    int8_t eq = 1;
    for (size_t i = 0; i < N; i++) {
        eq &= (a->coef[i] == b->coef[i]);
    }
    return eq;
}

int main(void) {
    uint8_t seed_keygen[32] = {0};
    uint8_t seed_encrypt[32] = {2};

    poly f; poly h; poly g; poly c; poly m = {{1}}; poly mpr;
    m.coef[4] = -1; m.coef[71] = -1; m.coef[423] = 1;

    print_poly("m", &m);
    ntru_hrss_keygen(&f, &h, seed_keygen);
    ntru_hrss_encrypt(&c, &m, &h, seed_encrypt);
    ntru_hrss_decrypt(&mpr, &c, &f);

    print_poly("mpr", &mpr);

    int8_t eq = 1;
    for(size_t i = 0; i < N; i++){
        eq &= (m.coef[i] == mpr.coef[i]);
    }

    if (eq){printf("Success!!\n");}

    return 1;
}
