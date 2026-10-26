#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "big_number.h"

#define MAX_DIGITS 20000
#define DIV_REPEATS 1000

long long get_time_ns(void)
{
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC, &t);

    return (long long)t.tv_sec * 1000000000LL + t.tv_nsec;
}

void make_number(char *s, int digits)
{
    s[0] = '8';

    for (int i = 1; i < digits; i++)
        s[i] = '7';

    s[digits] = '\0';
}

int main(void)
{
    char input[MAX_DIGITS + 1];

    FILE *file = fopen("../test_bench/benchmark.csv", "w");

    if (file == NULL) {
        perror("Cannot create benchmark.csv");
        return 1;
    }

    fprintf(file, "digits,bn_mul_ns,bn_divmod_small_ns,odd_sum_ns\n");

    /*
     * Dung bien volatile de dam bao ket qua duoc su dung.
     * Bien nay nam ngoai vung tinh thoi gian.
     */
    volatile uint32_t sink = 0;

    for (int digits = 100; digits <= MAX_DIGITS; digits += 100) {
        make_number(input, digits);

        bn_t n = bn_from_string(input);
        bn_t result;
        uint32_t remainder;

        long long start, end;
        double mul_time, div_time, odd_time;

        /*
         * Giam so lan lap khi so dau vao lon
         * de tranh benchmark chay qua lau.
         */
        int repeats;

        if (digits <= 1000)
            repeats = 20;
        else if (digits <= 5000)
            repeats = 5;
        else
            repeats = 2;

        /*
         * Benchmark bn_mul
         */
        start = get_time_ns();

        for (int i = 0; i < repeats; i++) {
            result = bn_mul(n, n);
            sink ^= result.limbs[0];
        }

        end = get_time_ns();

        mul_time = (double)(end - start) / repeats;

        /*
         * Benchmark bn_divmod_small
         */
        start = get_time_ns();

        for (int i = 0; i < DIV_REPEATS; i++) {
            bn_divmod_small(n, 3, &result, &remainder);
            sink ^= result.limbs[0];
            sink ^= remainder;
        }

        end = get_time_ns();

        div_time = (double)(end - start) / DIV_REPEATS;

        /*
         * Benchmark odd_sum voi power = 3
         */
        start = get_time_ns();

        for (int i = 0; i < repeats; i++) {
            result = odd_sum(n, 3);
            sink ^= result.limbs[0];
        }

        end = get_time_ns();

        odd_time = (double)(end - start) / repeats;

        fprintf(file, "%d,%.0f,%.0f,%.0f\n",
                digits, mul_time, div_time, odd_time);

        fflush(file);

        printf("Tested %d digits\n", digits);
    }

    fclose(file);

    printf("Benchmark completed. Results saved to benchmark.csv\n");

    /* Su dung sink sau khi benchmark */
    printf("Benchmark checksum: %u\n", (unsigned int)sink);

    return 0;
}