#include <stdio.h>
#include <string.h>
#include "poly.h"
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

int main(void) {
    poly a, inv, prod;

    poly_zero(&a);
    //polynôme quelconque
    /*
    a.coef[2] = 1;
    a.coef[5] = 1;
    a.coef[47] = -1;
    a.coef[477] = -1;
    */

    if (!poly_inv_3(&inv, &a)) {
        printf("Inverse non trouvé (a = 0)\n");
        return 1;
    }

    print_poly("a   ", &a);
    print_poly("inv ", &inv);

    poly_mul_3(&prod, &a, &inv);
    print_poly("a*inv", &prod);

    return 0;
}



