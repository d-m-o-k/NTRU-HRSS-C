#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "poly.h"
#include "param.h"

const poly NTRU_PHI = {{1}};


//fonctions inline de centrage dans {-1, 0, 1} et [-q/2, q/2 - 1] pour garantir les bons représentants lors des opérations
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


void poly_zero(poly *a){
    for (size_t i = 0; i < N; i++){
        a->coef[i] = 0;
    }
}

void poly_copy(poly *res, const poly *a){
    for (size_t i = 0; i < N; i++){
        res->coef[i] = a->coef[i];
    }
}

void poly_add_3(poly *res, const poly *a, const poly *b){
    for (size_t i = 0; i < N; i++){
        int64_t s = (a->coef[i] + b->coef[i]);
        res->coef[i] = centering_3(s);
    }
}

void poly_add_q(poly *res, const poly *a, const poly *b){
    for (size_t i = 0; i < N; i++){
        int64_t s = (a->coef[i] + b->coef[i]);
        res->coef[i] = centering_q(s);
    }
}

void poly_add_2(poly *res, const poly *a, const poly *b){
    for (size_t i = 0; i < N; i++){
        res->coef[i] = (a->coef[i] ^ b->coef[i]);
    }
}

void poly_neg_3(poly *res, const poly *a){
    for (size_t i = 0; i < N; i++){
        int64_t s = (- a->coef[i])%3;
        s -= 3 * (s == 2);
        s += 3 * (s == -2);
        res->coef[i] = s;
    }
}

void poly_neg_q(poly *res, const poly *a){
    for (size_t i = 0; i < N; i++){
        res->coef[i] = -(a->coef[i]); // car les coefficients sont supposés dans [-q/2, q/2 - 1] de base donc une simple négation suffit toujours sauf erreur antérieure de format
    }
}

void poly_sub_3(poly *res, const poly *a, const poly *b){
    poly_neg_3(res, b);
    poly_add_3(res, a, res);
}

void poly_sub_q(poly *res, const poly *a, const poly *b){
    poly_neg_q(res, b);
    poly_add_q(res, a, res);
}

void poly_mul_3(poly *res, const poly *a, const poly *b){
    int64_t buffer[2*N] = {0};

    // Produit brut
    for (size_t i = 0; i < 2*N - 1; i++) {
        size_t kmin = (i >= N) ? i - N + 1 : 0;
        size_t kmax = (i < N)  ? i : N - 1;
        for (size_t k = kmin; k <= kmax; k++) {
            buffer[i] += (int64_t)a->coef[k] * b->coef[i - k];
        }
    }

    // première réduction
    for (size_t i = 0; i < N - 1; i++) {
        buffer[i] += buffer[i + N];
    }

    // réduction mod Phi_N en utilisant X^N-1 = -(sum (X_i)_0=<i<N-1)
    for (size_t i = 0; i < N - 1; i++) {
        buffer[i] -= buffer[N - 1];
    }

    // écriture dans la variable de sortie
    for (size_t i = 0; i < N - 1; i++) {
        res->coef[i] = centering_3(buffer[i]);
    }
    res->coef[N - 1] = 0;
}

void poly_mul_q(poly *res, const poly *a, const poly *b){
    int64_t buffer[2*N] = {0};

    // Produit brut
    for (size_t i = 0; i < 2*N - 1; i++) {
        size_t kmin = (i >= N) ? i - N + 1 : 0;
        size_t kmax = (i < N)  ? i : N - 1;
        for (size_t k = kmin; k <= kmax; k++) {
            buffer[i] += (int64_t)a->coef[k] * b->coef[i - k];
        }
    }

    // première réduction
    for (size_t i = 0; i < N - 1; i++) {
        buffer[i] += buffer[i + N];
    }

    // réduction mod phi_N en utilisant X^(N-1) = -(sum (X_i)_(0=<i<N-1))
    for (size_t i = 0; i < N - 1; i++) {
        buffer[i] -= buffer[N - 1];
    }

    // écriture dans la variable de retour
    for (size_t i = 0; i < N - 1; i++) {
        res->coef[i] = centering_q(buffer[i]);
    }
    res->coef[N - 1] = 0;
}

void poly_mul_2(poly *res, const poly *a, const poly *b){
    int64_t buffer[2*N] = {0};

    // Produit brut
    for (size_t i = 0; i < 2*N - 1; i++) {
        size_t kmin = (i >= N) ? i - N + 1 : 0;
        size_t kmax = (i < N)  ? i : N - 1;
        for (size_t k = kmin; k <= kmax; k++) {
            buffer[i] += (int64_t)((a->coef[k]&1) & (b->coef[i - k]&1));
        }
    }

    // première réduction
    for (size_t i = 0; i < N - 1; i++) {
        buffer[i] ^= buffer[i + N];
    }

    // réduction mod Phi_N en utilisant X^N-1 = -(sum (X_i)_0=<i<N-1)
    for (size_t i = 0; i < N - 1; i++) {
        buffer[i] ^= buffer[N - 1];
    }

    // écriture dans la variable de sortie
    for (size_t i = 0; i < N - 1; i++) {
        res->coef[i] = buffer[i]&1;
    }
    res->coef[N - 1] = 0;
}

void poly_red_3(poly *res, const poly *a){
    for (size_t i = 0; i < N; i++){
        int16_t s = (a->coef[i])%3;
        res->coef[i] = centering_3(s);
    }
}

void poly_red_2(poly *res, const poly *a){
    for (size_t i = 0; i < N; i++){res->coef[i] = (a->coef[i])&1;}
}


// on implémente l'Almost-Inverse Algorithm en constant-time plutôt qu'Euclide étendu, version basée sur le papier de Venier-Cheung
// DOI: 10.1109/ICSPCC50002.2020.9259505
int poly_inv_3(poly *res, const poly *a){
    poly F, B, C, G;
    poly shiftr, shiftl;
    size_t k = 0;

    poly_copy(&F, a);
    poly_zero(&B); B.coef[0] = 1;
    poly_zero(&C);
    poly_zero(&G);
    for (size_t i = 0; i < N; i++){G.coef[i] = 1;}

    poly_zero(&shiftl);
    shiftl.coef[1] = 1; // X
    poly_zero(&shiftr);
    for (size_t j = 0; j < N - 1; j++){shiftr.coef[j] = -1;} // X^(N-1) + -(1 + X + ... + X^(N-2))

        // d = 1 ssi F = 1 (polynôme constant 1)
    int16_t d = (F.coef[0] == 1);
    for (size_t j = 1; j < N; j++) d &= (F.coef[j] == 0);

    poly FF, CC, BB;

    // Borne du papier pour le for
    size_t n_iter = 4 * (N - 1);

    for (size_t iter = 0; iter < n_iter; iter++) {
        // F̄ = rshift(F), C̄ = lshift(C), k̄ = k + 1
        poly_mul_3(&FF, &F, &shiftr);
        poly_mul_3(&CC, &C, &shiftl);
        size_t k_bar = k + 1;

        // m1 = d | (F0 != 0)
        int16_t f0_nz = (F.coef[0] != 0);
        int16_t m1 = d | f0_nz;
        int16_t mask1 = -m1;

        // F = F if m1 else F̄ ; C = C if m1 else C̄ ; k = k if m1 else k̄
        for (size_t j = 0; j < N; j++) {
            F.coef[j] = (F.coef[j] & mask1) | (FF.coef[j] & ~mask1);
            C.coef[j] = (C.coef[j] & mask1) | (CC.coef[j] & ~mask1);
        }
        k = (k & (size_t)mask1) | (k_bar & ~(size_t)mask1);

        // Recalcul de d = 1 ssi F = 1
        d = (F.coef[0] == 1);
        for (size_t j = 1; j < N; j++){d &= (F.coef[j] == 0);}

        // S = d | ~m1
        int16_t S = d | (1 - m1);
        int16_t maskS = -S;

        // cmp_f_g = (deg F >= deg G), calculé en constant-time
        int16_t cmp_f_g = 1, decided = 0;
        for (int j = N - 1; j >= 0; j--) {
            int16_t fnz = (F.coef[j] != 0);
            int16_t gnz = (G.coef[j] != 0);
            int16_t both = fnz & gnz;
            int16_t diff = fnz ^ gnz;
            int16_t take = diff & (1 - decided);
            int16_t take_eq = both & (1 - decided);
            int16_t mask_take = -take;
            int16_t mask_eq = -take_eq;
            cmp_f_g = (fnz & mask_take) | (1 & mask_eq) | (cmp_f_g & ~(mask_take | mask_eq));
            decided |= take | take_eq;
        }

        // m2 = S | cmp_f_g, on écvhange si m2 = 0
        int16_t m2 = S | cmp_f_g;
        int16_t mask2 = -m2;
        for (size_t j = 0; j < N; j++) {
            int16_t tF = F.coef[j], tG = G.coef[j];
            int16_t tB = B.coef[j], tC = C.coef[j];
            F.coef[j] = (tF & mask2) | (tG & ~mask2);
            G.coef[j] = (tG & mask2) | (tF & ~mask2);
            B.coef[j] = (tB & mask2) | (tC & ~mask2);
            C.coef[j] = (tC & mask2) | (tB & ~mask2);
        }

        // FF = F - G, BB = B - C
        poly_sub_3(&FF, &F, &G);
        poly_sub_3(&BB, &B, &C);

        // F = F if S else F̄ ; B = B if S else B̄
        for (size_t j = 0; j < N; j++) {
            F.coef[j] = (F.coef[j] & maskS) | (FF.coef[j] & ~maskS);
            B.coef[j] = (B.coef[j] & maskS) | (BB.coef[j] & ~maskS);
        }

        //recalcul de d encore
        d = (F.coef[0] == 1);
        for (size_t j = 1; j < N; j++) d &= (F.coef[j] == 0);
    }

    //élimination constant-time du facteur X^k dans l'almost-inverse par multiplication par X^-k
    size_t e = (N - (k % N)) % N;
    int16_t mask_last = -(e == N - 1);   // tout à 1 si e == N-1, sinon 0

    poly Xe;
    poly_zero(&Xe);
    for (size_t i = 0; i < N; i++) {
        int16_t is_e     = (i == e);
        int16_t is_lt_N1 = (i < N - 1);
        int16_t is_neg   = is_lt_N1 & mask_last;          // 1 si on doit mettre -1
        int16_t val      = (is_e & ~mask_last) | (-is_neg);
        Xe.coef[i] = val;
    }

    poly_mul_3(res, &B, &Xe);

    int output = (F.coef[0] == 1);
    for (size_t j = 1; j < N; j++){output &= (F.coef[j] == 0);}
    return output;
}

//même structure que la fonction précédente, quelques ajustements et simplifications à cause de la caractéristique 2
int poly_inv_2(poly *res, const poly *a){
    poly F, B, C, G;
    poly shiftr, shiftl;
    size_t k = 0;

    poly_copy(&F, a);
    poly_zero(&B); B.coef[0] = 1;
    poly_zero(&C);
    poly_zero(&G);
    for (size_t i = 0; i < N; i++) G.coef[i] = 1;

    poly_zero(&shiftl);
    shiftl.coef[1] = 1; // X
    poly_zero(&shiftr);
    for (size_t j = 0; j < N - 1; j++) shiftr.coef[j] = 1;   // X^(-1) mod (phi_N, 2)

    int16_t d = (F.coef[0] == 1);
    for (size_t j = 1; j < N; j++) d &= (F.coef[j] == 0);

    poly FF, CC, BB;

    size_t n_iter = 4 * (N - 1);

    for (size_t iter = 0; iter < n_iter; iter++) {
        poly_mul_2(&FF, &F, &shiftr);
        poly_mul_2(&CC, &C, &shiftl);
        size_t k_bar = k + 1;

        int16_t f0_nz = (F.coef[0] != 0);
        int16_t m1 = d | f0_nz;
        int16_t mask1 = -m1;

        for (size_t j = 0; j < N; j++) {
            F.coef[j] = (F.coef[j] & mask1) | (FF.coef[j] & ~mask1);
            C.coef[j] = (C.coef[j] & mask1) | (CC.coef[j] & ~mask1);
        }
        k = (k & (size_t)mask1) | (k_bar & ~(size_t)mask1);

        d = (F.coef[0] == 1);
        for (size_t j = 1; j < N; j++) d &= (F.coef[j] == 0);

        int16_t S = d | (1 - m1);
        int16_t maskS = -S;

        int16_t cmp_f_g = 1, decided = 0;
        for (int j = N - 1; j >= 0; j--) {
            int16_t fnz = (F.coef[j] != 0);
            int16_t gnz = (G.coef[j] != 0);
            int16_t both = fnz & gnz;
            int16_t diff = fnz ^ gnz;
            int16_t take = diff & (1 - decided);
            int16_t take_eq = both & (1 - decided);
            int16_t mask_take = -take;
            int16_t mask_eq = -take_eq;
            cmp_f_g = (fnz & mask_take) | (1 & mask_eq) | (cmp_f_g & ~(mask_take | mask_eq));
            decided |= take | take_eq;
        }

        int16_t m2 = S | cmp_f_g;
        int16_t mask2 = -m2;
        for (size_t j = 0; j < N; j++) {
            int16_t tF = F.coef[j], tG = G.coef[j];
            int16_t tB = B.coef[j], tC = C.coef[j];
            F.coef[j] = (tF & mask2) | (tG & ~mask2);
            G.coef[j] = (tG & mask2) | (tF & ~mask2);
            B.coef[j] = (tB & mask2) | (tC & ~mask2);
            C.coef[j] = (tC & mask2) | (tB & ~mask2);
        }

        // -x = x car en car 2
        poly_add_2(&FF, &F, &G);
        poly_add_2(&BB, &B, &C);

        for (size_t j = 0; j < N; j++) {
            F.coef[j] = (F.coef[j] & maskS) | (FF.coef[j] & ~maskS);
            B.coef[j] = (B.coef[j] & maskS) | (BB.coef[j] & ~maskS);
        }

        d = (F.coef[0] == 1);
        for (size_t j = 1; j < N; j++) d &= (F.coef[j] == 0);
    }

    size_t e = (N - (k % N)) % N;
    int16_t mask_last = -(e == N - 1);

    poly Xe;
    poly_zero(&Xe);
    for (size_t i = 0; i < N; i++) {
        int16_t is_e = (i == e);
        int16_t is_lt_N1 = (i < N - 1);
        int16_t val = (is_e & ~mask_last) | (is_lt_N1 & mask_last);
        Xe.coef[i] = val;
    }

    poly_mul_2(res, &B, &Xe);

    int output = (F.coef[0] == 1);
    for (size_t j = 1; j < N; j++){output &= (F.coef[j] == 0);}
    return output;
}

//fonctions de calcul d'un inverse modulo q (qu'on suppose égal à 8192 ici)
int poly_inv_q(poly *res, const poly *a){
    poly B;
    poly tempB;
    poly TWO = {0};
    TWO.coef[0] = 2;

    poly_red_2(&B, a);

    if(!poly_inv_2(&B, &B)){return 0;}

    for(size_t i = 0; i < 4; i++){
        poly_copy(&tempB,&B);
        poly_mul_q(&tempB, a, &B);
        poly_sub_q(&tempB, &TWO, &tempB);
        poly_mul_q(&B, &B, &tempB);
    }

    poly_copy(res, &B);
    return 1;
}

