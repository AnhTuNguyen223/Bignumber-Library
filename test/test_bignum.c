
#include <stdio.h>
#include <string.h>
#include "../src/big_number.h"

int main(void)
{
    bn_t a = bn_from_string("123");
    bn_t b = bn_from_string("456");
    bn_t result;
    char output[100];

    result = bn_add(a, b);
    bn_normalize(&result);
    bn_to_string(&result, output);
    printf("123 + 456 = %s\n", output);
    if (strcmp(output, "579") != 0) return 1;

    result = bn_sub(b, a);
    bn_normalize(&result);
    bn_to_string(&result, output);
    printf("456 - 123 = %s\n", output);
    if (strcmp(output, "333") != 0) return 1;

    result = bn_mul(a, b);
    bn_normalize(&result);
    bn_to_string(&result, output);
    printf("123 * 456 = %s\n", output);
    if (strcmp(output, "56088") != 0) return 1;

    bn_divmod_small(b, 3, &result, &(uint32_t){0});
    bn_normalize(&result);
    bn_to_string(&result, output);
    printf("456 / 3 = %s\n", output);
    if (strcmp(output, "152") != 0) return 1;

    printf("All Big Number tests passed!\n");
    return 0;
}