
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../src/big_number.h"

static int total_tests = 0;
static int passed_tests = 0;

static int check_result(const char *test_name,
                        bn_t actual,
                        const char *expected)
{
    char buffer[MAX_LIMBS * 9 + 2];

    bn_to_string(&actual, buffer);
    total_tests++;

    if (strcmp(buffer, expected) == 0) {
        printf("[PASS] %s\n", test_name);
        passed_tests++;
        return 1;
    }

    printf("[FAIL] %s\n", test_name);
    printf("       Expected: %s\n", expected);
    printf("       Actual:   %s\n", buffer);
    return 0;
}

static int test_add(void)
{
    int result = 1;
    bn_t a;
    bn_t b;
    bn_t actual;

    printf("\n=== Addition tests ===\n");

    a = bn_from_string("0");
    b = bn_from_string("0");
    actual = bn_add(a, b);
    result &= check_result("0 + 0", actual, "0");

    a = bn_from_string("999999999");
    b = bn_from_string("1");
    actual = bn_add(a, b);
    result &= check_result("Carry across one limb", actual, "1000000000");

    a = bn_from_string("999999999999999999");
    b = bn_from_string("1");
    actual = bn_add(a, b);
    result &= check_result("Carry across multiple limbs",
                           actual, "1000000000000000000");

    a = bn_from_string("999999999999999999999999999");
    b = bn_from_string("1");
    actual = bn_add(a, b);
    result &= check_result("Carry across three limbs",
                           actual, "1000000000000000000000000000");

    a = bn_from_string("123456789012345678901234567890");
    b = bn_from_string("987654321098765432109876543210");
    actual = bn_add(a, b);
    result &= check_result("Addition of large numbers",
                           actual, "1111111110111111111011111111100");

    return result;
}

static int test_sub(void)
{
    int result = 1;
    bn_t a;
    bn_t b;
    bn_t actual;

    printf("\n=== Subtraction tests ===\n");

    a = bn_from_string("123456789");
    b = bn_from_string("123456789");
    actual = bn_sub(a, b);
    result &= check_result("Equal numbers subtraction", actual, "0");

    a = bn_from_string("1000000000");
    b = bn_from_string("1");
    actual = bn_sub(a, b);
    result &= check_result("Borrow across one limb", actual, "999999999");

    a = bn_from_string("1000000000000000000");
    b = bn_from_string("1");
    actual = bn_sub(a, b);
    result &= check_result("Borrow across multiple limbs",
                           actual, "999999999999999999");

    a = bn_from_string("1000000000000000000000000000");
    b = bn_from_string("1");
    actual = bn_sub(a, b);
    result &= check_result("Borrow across three limbs",
                           actual, "999999999999999999999999999");

    a = bn_from_string("987654321098765432109876543210");
    b = bn_from_string("123456789012345678901234567890");
    actual = bn_sub(a, b);
    result &= check_result("Subtraction of large numbers",
                           actual, "864197532086419753208641975320");

    return result;
}

static int test_mul(void)
{
    int result = 1;
    bn_t a;
    bn_t b;
    bn_t actual;

    printf("\n=== Multiplication tests ===\n");

    a = bn_from_string("0");
    b = bn_from_string("12345678901234567890");
    actual = bn_mul(a, b);
    result &= check_result("Zero multiplication", actual, "0");

    a = bn_from_string("1");
    b = bn_from_string("12345678901234567890");
    actual = bn_mul(a, b);
    result &= check_result("Multiplication by one",
                           actual, "12345678901234567890");

    a = bn_from_string("999999999");
    b = bn_from_string("999999999");
    actual = bn_mul(a, b);
    result &= check_result("Maximum single-limb multiplication",
                           actual, "999999998000000001");

    a = bn_from_string("999999999999999999");
    b = bn_from_string("999999999999999999");
    actual = bn_mul(a, b);
    result &= check_result("Multiple-limb multiplication",
                           actual, "999999999999999998000000000000000001");

    a = bn_from_string("12345678901234567890");
    b = bn_from_string("9876543210");
    actual = bn_mul(a, b);
    result &= check_result("Multiplication of large numbers",
                           actual, "121932631124828532111263526900");

    return result;
}

static int test_compare(void)
{
    int result = 1;
    bn_t a;
    bn_t b;
    int comparison;

    printf("\n=== Comparison tests ===\n");

    a = bn_from_string("12345678901234567890");
    b = bn_from_string("12345678901234567890");
    comparison = bn_compare(a, b);
    total_tests++;

    if (comparison == 2) {
        printf("[PASS] Equal numbers comparison\n");
        passed_tests++;
    } else {
        printf("[FAIL] Equal numbers comparison: got %d\n", comparison);
        result = 0;
    }

    a = bn_from_string("1000000000000000000");
    b = bn_from_string("999999999999999999");
    comparison = bn_compare(a, b);
    total_tests++;

    if (comparison == 1) {
        printf("[PASS] Greater numbers comparison\n");
        passed_tests++;
    } else {
        printf("[FAIL] Greater numbers comparison: got %d\n", comparison);
        result = 0;
    }

    a = bn_from_string("999999999999999999");
    b = bn_from_string("1000000000000000000");
    comparison = bn_compare(a, b);
    total_tests++;

    if (comparison == 0) {
        printf("[PASS] Smaller numbers comparison\n");
        passed_tests++;
    } else {
        printf("[FAIL] Smaller numbers comparison: got %d\n", comparison);
        result = 0;
    }

    return result;
}

int main(void)
{
    int result = 1;

    printf("Big Number capacity and arithmetic tests\n");

    result &= test_add();
    result &= test_sub();
    result &= test_mul();
    result &= test_compare();

    printf("\n=== Test summary ===\n");
    printf("Passed: %d/%d\n", passed_tests, total_tests);
    printf("Failed: %d/%d\n", total_tests - passed_tests, total_tests);

    return result ? 0 : 1;
}
