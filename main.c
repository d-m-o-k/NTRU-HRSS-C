#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "poly.h"
#include "coding.h"
#include "sample.h"
#include "ntru.h"
#include "param.h"
#include "rng.h"

static void hex_to_bytes(const char *hex, uint8_t *out, size_t len) {
    for (size_t i = 0; i < len; i++) {
        unsigned int val;
        if (sscanf(hex + 2 * i, "%2x", &val) != 1) {
            val = 0;
        }
        out[i] = (uint8_t)val;
    }
}

static void bytes_to_hex(char *out, const uint8_t *in, size_t len) {
    for (size_t i = 0; i < len; i++) {
        sprintf(out + 2 * i, "%02X", in[i]);
    }
    out[2 * len] = '\0';
}

static int parse_req_seed(const char *filename, int target_count, uint8_t seed[48]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Erreur d'ouverture du fichier .req");
        return 0;
    }

    char line[1024];
    int current_count = -1;
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        // Détecte le début d'un bloc de test
        if (sscanf(line, "count = %d", &current_count) == 1) {
            if (current_count == target_count) {
                found = 1;
            } else if (found) {
                break;
            }
        }

        // Dès qu'on a le bon count, on extrait la seed
        if (found && strncmp(line, "seed = ", 7) == 0) {
            hex_to_bytes(line + 7, seed, 48);
            break;
        }
    }

    fclose(fp);
    return found;
}

static void run_kat_test(int count, const char *req_file, const char *ans_file) {
    uint8_t seed[48] = {0};
    uint8_t seed_keygen[32];
    uint8_t seed_encrypt[32];

    printf("Recherche du test count = %d dans %s...\n", count, req_file);
    if (!parse_req_seed(req_file, count, seed)) {
        printf("ERREUR Impossible de trouver le count = %d dans le fichier.\n", count);
        return;
    }
    printf("Seed trouvée\n");

    // Initialisation du DRBG NIST
    randombytes_init(seed, NULL, 256);
    randombytes(seed_keygen, 32);
    randombytes(seed_encrypt, 32);

    poly f, h, r, m, c, f_inv_3;

    // KeyGen (utilise la seed de 32 octets générée par le DRBG)
    if (!ntru_hrss_keygen(&f, &h, seed_keygen)) {
        printf("ERREUR La génération de clé a échoué.\n");
        return;
    }

    // Encodage de la clé publique
    uint8_t pk_bytes[NTRU_PUBLICKEYBYTES];
    Rq_encode(pk_bytes, &h);

    // On construit la clé secrète au format standard
    // sk = S3_encode(f) || S3_encode(f^-1 mod 3) || pk
    uint8_t sk_bytes[NTRU_SECRETKEYBYTES] = {0};
    uint8_t f_enc[140];
    uint8_t f_inv_enc[140];

    poly_inv_3(&f_inv_3, &f); // f est toujours inversible par construction

    S3_encode(f_enc, &f);
    S3_encode(f_inv_enc, &f_inv_3);

    memcpy(sk_bytes, f_enc, 140);
    memcpy(sk_bytes + 140, f_inv_enc, 140);
    memcpy(sk_bytes + 280, pk_bytes, NTRU_PUBLICKEYBYTES);
    // Le reste est paddé à 0

    // Sampling de r et m à partir de la seed (gère le SHAKE de 1408 octets en interne)
    sample_KAT_rm(&r, &m, seed_encrypt);

    // Chiffrement hrss c = h*r + m (q)
    poly_mul_q(&c, &h, &r);
    poly_add_q(&c, &c, &m);

    // Encodage du chiffré
    uint8_t ct_bytes[NTRU_CIPHERTEXTBYTES];
    Rq_encode(ct_bytes, &c);

    // On écrit dans le fichier .ans
    FILE *fp_ans = fopen(ans_file, "a");
    if (!fp_ans) {
        perror("ERREUR impossible d'ouvrir le fichier .ans");
        return;
    }

    // écriture HEXA
    char *hex_pk = malloc(NTRU_PUBLICKEYBYTES * 2 + 1);
    char *hex_sk = malloc(NTRU_SECRETKEYBYTES * 2 + 1);
    char *hex_ct = malloc(NTRU_CIPHERTEXTBYTES * 2 + 1);
    char hex_seed[97];

    bytes_to_hex(hex_pk, pk_bytes, NTRU_PUBLICKEYBYTES);
    bytes_to_hex(hex_sk, sk_bytes, NTRU_SECRETKEYBYTES);
    bytes_to_hex(hex_ct, ct_bytes, NTRU_CIPHERTEXTBYTES);
    bytes_to_hex(hex_seed, seed, 48);

    fprintf(fp_ans, "count = %d\n", count);
    fprintf(fp_ans, "seed = %s\n", hex_seed);
    fprintf(fp_ans, "pk = %s\n", hex_pk);
    fprintf(fp_ans, "sk = %s\n", hex_sk);
    fprintf(fp_ans, "ct = %s\n\n", hex_ct);

    fclose(fp_ans);
    free(hex_pk);
    free(hex_sk);
    free(hex_ct);

    printf("Test count = %d écrit avec succès dans %s\n", count, ans_file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <count>\n", argv[0]);
        printf("Exemple: %s 0\n", argv[0]);
        return 1;
    }

    int target_count = atoi(argv[1]);

    const char *req_file = "PQCkemKAT_1450.req";
    const char *ans_file = "PQCkemKAT_1450.ans";

    run_kat_test(target_count, req_file, ans_file);

    return 0;
}
