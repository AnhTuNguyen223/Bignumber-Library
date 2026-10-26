#ifndef BIG_NUMBER_H
#define BIG_NUMBER_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_LIMBS 10000
#define BN_BASE 1000000000

typedef struct {
    uint32_t limbs[MAX_LIMBS];
    size_t len;
} bn_t;

// Chuyen doi chuoi va Big Number 
bn_t bn_from_string(char *p);
void bn_to_string(bn_t *bn, char *str_out );

// Cac phep toan Big Number 
int bn_compare(bn_t a, bn_t b);
bn_t bn_add(bn_t a, bn_t b);
bn_t bn_sub(bn_t a, bn_t b);
bn_t bn_mul(bn_t a, bn_t b);
void bn_divmod_small(bn_t a, uint32_t divisor,
                     bn_t *quotient, uint32_t *remainder);

// Chuan hoa va tinh tong luy thua cac so le 
void bn_normalize(bn_t *bn);
bn_t odd_sum(bn_t bn, uint8_t power);

#endif