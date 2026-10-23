#include <stdio.h>
#include <string.h>
#include "../src/big_number.h"

int main(void)
{
    bn_t n, result;
    char output[1000];

    /* Test 1: N = 3, power = 1
       1 + 3 = 4
    */
    n = bn_from_string("3");
    result = odd_sum(n, 1);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(3, 1) = %s\n", output);
    if (strcmp(output, "4") != 0) return 1;

    /* Test 2: N = 3, power = 2
       1^2 + 3^2 = 10
    */
    result = odd_sum(n, 2);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(3, 2) = %s\n", output);
    if (strcmp(output, "10") != 0) return 1;

    /* Test 3: N = 3, power = 3
       1^3 + 3^3 = 28
    */
    result = odd_sum(n, 3);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(3, 3) = %s\n", output);
    if (strcmp(output, "28") != 0) return 1;

    /* Test 4: N = 9, power = 1
       1 + 3 + 5 + 7 + 9 = 25
    */
    n = bn_from_string("9");
    result = odd_sum(n, 1);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(9, 1) = %s\n", output);
    if (strcmp(output, "25") != 0) return 1;

    /* Test 5: N = 9, power = 2
       1 + 9 + 25 + 49 + 81 = 165
    */
    result = odd_sum(n, 2);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(9, 2) = %s\n", output);
    if (strcmp(output, "165") != 0) return 1;

    /* Test 6: N = 9, power = 3
       1 + 27 + 125 + 343 + 729 = 1225
    */
    result = odd_sum(n, 3);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(9, 3) = %s\n", output);
    if (strcmp(output, "1225") != 0) return 1;

    /* Test 7: N = 10, power = 1
       1 + 3 + 5 + 7 + 9 = 25
    */
    n = bn_from_string("10");
    result = odd_sum(n, 1);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(10, 1) = %s\n", output);
    if (strcmp(output, "25") != 0) return 1;

    /* Test 8: N = 0 */
    n = bn_from_string("0");
    result = odd_sum(n, 3);
    bn_normalize(&result);
    bn_to_string(&result, output);

    printf("odd_sum(0, 3) = %s\n", output);
    if (strcmp(output, "0") != 0) return 1;

    printf("All odd_sum tests passed!\n");

    return 0;
}