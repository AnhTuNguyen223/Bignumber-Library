
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../src/big_number.h"

static int tests_run = 0;
static int tests_failed = 0;

/* Chuyen Big Number thanh chuoi va so sanh voi ket qua mong doi */
static void check_result(
    const char *test_name,
    bn_t result,
    const char *expected)
{
    char output[20001];

    bn_normalize(&result);
    bn_to_string(&result, output);

    tests_run++;

    if (strcmp(output, expected) == 0) {
        printf("[PASS] %s = %s\n", test_name, output);
    } else {
        printf("[FAIL] %s\n", test_name);
        printf("       Expected: %s\n", expected);
        printf("       Actual:   %s\n", output);
        tests_failed++;
    }
}

/* Kiem tra chuyen chuoi sang Big Number */
static void test_from_string(void)
{
    printf("\n===== TEST bn_from_string =====\n");

    check_result(
        "Convert 0",
        bn_from_string("0"),
        "0");

    check_result(
        "Convert 123",
        bn_from_string("123"),
        "123");

    check_result(
        "Convert 123456789",
        bn_from_string("123456789"),
        "123456789");

    check_result(
        "Convert 123456789012",
        bn_from_string("123456789012"),
        "123456789012");

    check_result(
        "Convert 123456789012345678",
        bn_from_string("123456789012345678"),
        "123456789012345678");

    check_result(
        "Convert 20 digits",
        bn_from_string("12345678901234567890"),
        "12345678901234567890");
}

/* Kiem tra phep cong */
static void test_add(void)
{
    printf("\n===== TEST bn_add =====\n");

    check_result(
        "123 + 456",
        bn_add(
            bn_from_string("123"),
            bn_from_string("456")),
        "579");

    check_result(
        "999 + 1",
        bn_add(
            bn_from_string("999"),
            bn_from_string("1")),
        "1000");

    check_result(
        "999999999 + 1",
        bn_add(
            bn_from_string("999999999"),
            bn_from_string("1")),
        "1000000000");

    check_result(
        "999999999999999999 + 1",
        bn_add(
            bn_from_string("999999999999999999"),
            bn_from_string("1")),
        "1000000000000000000");

    check_result(
        "12345678901234567890 + 98765432109876543210",
        bn_add(
            bn_from_string("12345678901234567890"),
            bn_from_string("98765432109876543210")),
        "111111111011111111100");

    check_result(
        "0 + 123456789",
        bn_add(
            bn_from_string("0"),
            bn_from_string("123456789")),
        "123456789");
}

/* Kiem tra phep tru */
static void test_sub(void)
{
    printf("\n===== TEST bn_sub =====\n");

    check_result(
        "456 - 123",
        bn_sub(
            bn_from_string("456"),
            bn_from_string("123")),
        "333");

    check_result(
        "1000 - 1",
        bn_sub(
            bn_from_string("1000"),
            bn_from_string("1")),
        "999");

    check_result(
        "1000000000 - 1",
        bn_sub(
            bn_from_string("1000000000"),
            bn_from_string("1")),
        "999999999");

    check_result(
        "1000000000000000000 - 1",
        bn_sub(
            bn_from_string("1000000000000000000"),
            bn_from_string("1")),
        "999999999999999999");

    check_result(
        "12345678901234567890 - 1234567890",
        bn_sub(
            bn_from_string("12345678901234567890"),
            bn_from_string("1234567890")),
        "12345678900000000000");

    check_result(
        "123 - 123",
        bn_sub(
            bn_from_string("123"),
            bn_from_string("123")),
        "0");

    check_result(
        "123 - 456",
        bn_sub(
            bn_from_string("123"),
            bn_from_string("456")),
        "0");
}

/* Kiem tra phep nhan */
static void test_mul(void)
{
    printf("\n===== TEST bn_mul =====\n");

    check_result(
        "123 * 456",
        bn_mul(
            bn_from_string("123"),
            bn_from_string("456")),
        "56088");

    check_result(
        "999999999 * 999999999",
        bn_mul(
            bn_from_string("999999999"),
            bn_from_string("999999999")),
        "999999998000000001");

    check_result(
        "1000000000 * 1000000000",
        bn_mul(
            bn_from_string("1000000000"),
            bn_from_string("1000000000")),
        "1000000000000000000");

    check_result(
        "12345678901234567890 * 2",
        bn_mul(
            bn_from_string("12345678901234567890"),
            bn_from_string("2")),
        "24691357802469135780");

    check_result(
        "123456789 * 987654321",
        bn_mul(
            bn_from_string("123456789"),
            bn_from_string("987654321")),
        "121932631112635269");

    check_result(
        "0 * 123456789",
        bn_mul(
            bn_from_string("0"),
            bn_from_string("123456789")),
        "0");
}

/* Kiem tra phep chia cho so nguyen nho */
static void test_divmod_small(void)
{
    printf("\n===== TEST bn_divmod_small =====\n");

    bn_t quotient;
    uint32_t remainder;

    bn_divmod_small(
        bn_from_string("456"),
        3,
        &quotient,
        &remainder);

    check_result("456 / 3 quotient", quotient, "152");

    tests_run++;
    if (remainder == 0) {
        printf("[PASS] 456 / 3 remainder = 0\n");
    } else {
        printf("[FAIL] 456 / 3 remainder\n");
        printf("       Expected: 0\n");
        printf("       Actual:   %u\n", remainder);
        tests_failed++;
    }

    bn_divmod_small(
        bn_from_string("1000"),
        3,
        &quotient,
        &remainder);

    check_result("1000 / 3 quotient", quotient, "333");

    tests_run++;
    if (remainder == 1) {
        printf("[PASS] 1000 / 3 remainder = 1\n");
    } else {
        printf("[FAIL] 1000 / 3 remainder\n");
        printf("       Expected: 1\n");
        printf("       Actual:   %u\n", remainder);
        tests_failed++;
    }

    bn_divmod_small(
        bn_from_string("1000000000000000000"),
        2,
        &quotient,
        &remainder);

    check_result(
        "1000000000000000000 / 2 quotient",
        quotient,
        "500000000000000000");

    tests_run++;
    if (remainder == 0) {
        printf("[PASS] Large division remainder = 0\n");
    } else {
        printf("[FAIL] Large division remainder\n");
        printf("       Expected: 0\n");
        printf("       Actual:   %u\n", remainder);
        tests_failed++;
    }
}

/* Kiem tra so sanh */
static void test_compare(void)
{
    printf("\n===== TEST bn_compare =====\n");

    tests_run++;
    if (bn_compare(
            bn_from_string("456"),
            bn_from_string("123")) == 1) {
        printf("[PASS] 456 > 123\n");
    } else {
        printf("[FAIL] 456 > 123\n");
        tests_failed++;
    }

    tests_run++;
    if (bn_compare(
            bn_from_string("123"),
            bn_from_string("456")) == 0) {
        printf("[PASS] 123 < 456\n");
    } else {
        printf("[FAIL] 123 < 456\n");
        tests_failed++;
    }

    tests_run++;
    if (bn_compare(
            bn_from_string("123456789"),
            bn_from_string("123456789")) == 2) {
        printf("[PASS] 123456789 == 123456789\n");
    } else {
        printf("[FAIL] 123456789 == 123456789\n");
        tests_failed++;
    }

    tests_run++;
    if (bn_compare(
            bn_from_string("999999999"),
            bn_from_string("1000000000")) == 0) {
        printf("[PASS] Different limb counts\n");
    } else {
        printf("[FAIL] Different limb counts\n");
        tests_failed++;
    }
}

int main(void)
{
    test_from_string();
    test_add();
    test_sub();
    test_mul();
    test_divmod_small();
    test_compare();

    printf("\n=================================\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests failed: %d\n", tests_failed);

    if (tests_failed == 0) {
        printf("All Big Number tests passed!\n");
        return 0;
    }

    printf("Some Big Number tests failed!\n");
    return 1;
}
