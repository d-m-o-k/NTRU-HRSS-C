#ifndef NTRU_H
#define NTRU_H
#include <stdint.h>
#include "param.h"
#include "poly.h"
#include "fips202.h"
#include "coding.h"
#include "sample.h"

//NTRU-HRSS-Encrypt, API Interne
int ntru_hrss_keygen(poly *f, poly *h, const uint8_t seed[32]); // KeyGen pour NTRU-HRSS, retourne la clé privée f et la clé publique h = (X-1)g(1/f)_(q))
void ntru_hrss_encrypt(poly *c, const poly *m, const  poly *h, const uint8_t seed[32]); // retourne c = h*r + m [q], r généré aléatoirement via la seed
int ntru_hrss_decrypt(poly *m, const poly *c, const poly *f); // retourne m à partir d'un chiffré comme défini précédemment et la clé secrète f

//KAT
void ntru_hrss_KAT_rmgen(poly *m, poly *r, const uint8_t seed[32]);

// NTRU-HRSS-KEM
int ntru_hrss_keygen_KEM();
void ntru_hrss_encapsulate();
void ntru_hrss_decapsulate();



#endif
