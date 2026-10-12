#ifndef CODING_H
#define CODING_H
#include <stdint.h>
#include "param.h"
#include "poly.h"
#include "fips202.h"

void Rq_encode(int8_t bits[(N+7)/8], poly *a);
void Rq_decode(int8_t bits[(N+7)/8], poly *a);
void S3_encode(int8_t bits[(N+7)/8], poly *a);
void S3_decode(int8_t bits[(N+7)/8], poly *a);

#endif
