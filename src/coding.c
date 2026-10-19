#include <stdio.h>
#include <stdlib.h>
#include "fips202.h"
#include "param.h"
#include "poly.h"

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


void Rq_encode(int8_t bits[NTRU_CIPHERTEXTBYTES ], const poly *a){
    //on initialise le buffer à 0
    for (size_t i = 0; i < NTRU_CIPHERTEXTBYTES; i++){
        bits[i] = 0;
    }

    size_t bit_id = 0; //index des bits écrits

    for(size_t i = 0; i < N - 1; i++){
        uint16_t v = a->coef[i] & (q - 1); // on ramène le coefficient dans [0, q - 1]

        for (int j = 12; j >= 0; j--){
            uint8_t bit = (v >> j) & 1;

            // variables de position dans le bitstring, quels int8_t et quel bit de cet int8_t
            size_t byte_id = bit_id / 8;
            size_t byte_pos = bit_id % 8;

            // écriture
            bits[byte_id] |= (bit << byte_pos);
            bit_id++;
        }
    }
}

void Rq_decode(poly *a, const uint8_t in[NTRU_CIPHERTEXTBYTES]) {
    size_t bit_id = 0;
    int32_t sum = 0; // pour le calcul de v_{n-1}

    for (size_t i = 0; i < N - 1; i++) {
        uint16_t v = 0;

        // Lecture des 13 bits
        for (int j = 12; j >= 0; j--) {
            size_t byte_id = bit_id / 8;
            size_t byte_pos = bit_id % 8;
            uint8_t bit = (in[byte_id] >> byte_pos) & 1;

            v |= (bit << j);
            bit_id++;
        }

        // écriture et accumulation pour v_{n-1}
        a->coef[i] = centering_q(v);
        sum -= v;
    }

    uint16_t v_last = (q + (sum % q)) % q;
    a->coef[N - 1] = centering_q(v_last);
}

#define S3_BYTES ((N - 1) / 5)   // 140 pour N = 701

void S3_encode(uint8_t bits[S3_BYTES], const poly *a) {
    poly red_a;
    poly_red_3(&red_a, a);

    for (size_t i = 0; i < S3_BYTES; i++) {
        uint8_t byte = 0;
        for (int j = 0; j < 5; j++) {
            int16_t v = red_a.coef[5 * i + j];
            uint8_t c = (uint8_t)((v + 3) % 3);   // conversion de représentation centrée -> [0,2]
            byte = (uint8_t)(byte * 3 + c);
        }
        bits[i] = byte;
    }
}

void S3_decode(poly *a, const uint8_t in[S3_BYTES]) {
    poly_zero(a);

    for (size_t i = 0; i < S3_BYTES; i++) {
        uint8_t byte = in[i];
        for (int j = 4; j >= 0; j--) {
            uint8_t c = byte % 3;
            byte /= 3;
            a->coef[5 * i + j] = (c == 2) ? -1 : (int16_t)c;
        }
    }
}
