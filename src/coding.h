#ifndef CODING_H
#define CODING_H
#include <stdint.h>
#include "param.h"
#include "poly.h"
#include "fips202.h"

void Rq_encode(int8_t bits[NTRU_CIPHERTEXTBYTES], const poly *a);
void Rq_decode(const poly *a, int8_t in[NTRU_CIPHERTEXTBYTES]);
void S3_encode(int8_t bits[((N - 1) / 5)], const poly *a);
void S3_decode(const poly *a, int8_t bits[((N - 1) / 5)]);

#endif
