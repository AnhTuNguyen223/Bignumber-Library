CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wconversion -pedantic -Werror

TARGET = odd_sum
SRCS = main.c big_number.c
OBJS = $(SRCS:.c=.o)


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c big_number.h
	$(CC) $(CFLAGS) -c $< -o $@

# Run all tests
.PHONY: test
test: test_bignum_library $(TARGET)
	./test_bignum_library
	python3 ../test/test_bignum_oddsum.py
	python3 ../test/test_edge.py
# Run test edge
.PHONY: edgetest
edgetest: 
	python3 ../test/test_edge.py
#Run test odd_sum
.PHONY: test_odd_sum
test_odd_sum: 
	python3 ../test/test_bignum_oddsum.py
#Run test Library
.PHONY: test_library
test_library: test_bignum_library
	./test_bignum_library
test_bignum_library: ../test/test_bignum_library.c big_number.c big_number.h
	$(CC) $(CFLAGS) ../test/test_bignum_library.c big_number.c -o test_bignum_library

BENCH_SRC = ../test_bench/benchmark.c
BENCH_TARGET = benchmark

.PHONY: bench clean-bench

$(BENCH_TARGET): $(BENCH_SRC) big_number.c big_number.h
	$(CC) $(CFLAGS) -O2 -I. $(BENCH_SRC) big_number.c -o $(BENCH_TARGET)
# Run testbench then run draw log-log
bench: $(BENCH_TARGET)
	./$(BENCH_TARGET)
	cd ../test_bench && python3 plot_benchmark.py

clean-bench:
	rm -f $(BENCH_TARGET) ../test_bench/benchmark.csv
clean:
	rm -f $(OBJS) $(TARGET) test_bignum test_odd_sum