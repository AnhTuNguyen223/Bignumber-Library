#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "big_number.h"

int main(int argc, char *argv[]){
    uint8_t power = 1;  // Mac dinh power = 1

    if (argc == 4) {
    if (strcmp(argv[2], "--power") != 0) {
        printf("Usage: %s file_input [--power p]\n", argv[0]);
        return 1;
    }

    if (strlen(argv[3]) != 1 ||
        argv[3][0] < '1' ||
        argv[3][0] > '3') {
        printf("Error: power must be 1, 2, or 3\n");
        return 1;
    }

    power = (uint8_t)(argv[3][0] - '0');

    } else if (argc != 2) {
        printf("Usage: %s file_input [--power p]\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        perror("Cannot open input file");
        return 1;
    }
    char input[20001];

    if (fscanf(file, "%20000s", input) != 1) {
        printf("Input file is empty or invalid\n");
        fclose(file);
        return 1;
    }
    char string_out[200001];
    bn_t N = bn_from_string(input);
    bn_to_string(&N, string_out);
    printf("N: %s\n", string_out);

    char bn[] = "1234567890123" ;
    char bn1[] = "341234567890123" ;

    uint32_t remainder=0;
    bn_t big_number_1;
    bn_t big_number_2;
    bn_t big_number_3;
    bn_t odd_sum_bn;
    big_number_1=bn_from_string(&bn[0]);
    big_number_2=bn_from_string(&bn1[0]);
    big_number_3 = bn_add(N,big_number_2);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER ADD:%s\n", string_out);

    int cp = bn_compare(N,big_number_2);
    if (cp == 2)
        printf("BN1 == BN 2 \n");
    else if (cp == 1)
        printf("BN1 > BN 2 \n");
    else
        printf("BN1 < BN \n");

    big_number_3 = bn_sub(N,big_number_1);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER SUB:%s\n", string_out);

    big_number_3 = bn_mul(N, big_number_2);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER MUL:%s\n", string_out);

    bn_divmod_small(N, 3, &big_number_3, &remainder);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER DIVMOD:%s\n", string_out);

    odd_sum_bn = odd_sum(N, power);
    bn_normalize(&odd_sum_bn);
    bn_to_string(&odd_sum_bn, string_out);
    printf("BIGNUMBER odd sum:%s\n", string_out);

    return 0;
}
