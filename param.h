#ifndef PARAM_H
#define PARAM_H

// constantes et paramètres
#define NTRU_N 701 // premier pour lequel l'ordre de 2 et 3 dans F_N* est N-1
#define NTRU_P 3
#define NTRU_Q 8192
#define NTRU_LOGQ 13
#define NTRU_WEIGHT 0

// alias pour la rapidité
#define N NTRU_N
#define q NTRU_Q
#define p NTRU_P

// taille des buffers
#define NTRU_PUBLICKEYBYTES 1138
#define NTRU_SECRETKEYBYTES 1450
#define NTRU_CIPHERTEXTBYTES 1138

#endif
