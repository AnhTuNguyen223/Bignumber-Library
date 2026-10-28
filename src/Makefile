
CC = gcc
PYTHON = python3

CFLAGS = -std=c11 -Wall -Wextra -Wconversion -pedantic -Werror
OPT_CFLAGS = $(CFLAGS) -O2
SHARED_CFLAGS = $(OPT_CFLAGS) -fPIC
SANITIZER_CFLAGS = -std=c11 -Wall -Wextra -Wconversion -pedantic \
                   -g -O1 -fsanitize=address,undefined \
                   -fno-omit-frame-pointer

TARGET = odd_sum
SRCS = main.c big_number.c
OBJS = $(SRCS:.c=.o)

SHARED_LIB = ../libbig_number.so

TEST_LIBRARY_TARGET = test_bignum_library
TEST_CAPACITY_TARGET = test_capacity_bin
TEST_CAPACITY_SANITIZE_TARGET = test_capacity_sanitize_bin
SANITIZED_TARGET = odd_sum_sanitize

BENCH_SRC = ../test_bench/benchmark.c
BENCH_TARGET = benchmark

.PHONY: all clean test \
        test_library test_lib test_odd_sum test_edge edgetest \
        test_properties test_capacity test_capacity_sanitize \
        bench clean-bench

# Default target
all: $(TARGET)

# Main application
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Build main application with ASan and UBSan
$(SANITIZED_TARGET): main.c big_number.c big_number.h
	$(CC) $(SANITIZER_CFLAGS) main.c big_number.c -o $@

# Compile object files
%.o: %.c big_number.h
	$(CC) $(CFLAGS) -c $< -o $@

# Shared library for Python ctypes
$(SHARED_LIB): big_number.c big_number.h
	$(CC) $(SHARED_CFLAGS) -shared big_number.c -o $@

# C library test executable
$(TEST_LIBRARY_TARGET): ../test/test_bignum_library.c big_number.c big_number.h
	$(CC) $(CFLAGS) ../test/test_bignum_library.c big_number.c -o $@

# Capacity test executable
$(TEST_CAPACITY_TARGET): ../test/test_capacity.c big_number.c big_number.h
	$(CC) $(CFLAGS) ../test/test_capacity.c big_number.c -o $@

# Capacity test executable with AddressSanitizer and UndefinedBehaviorSanitizer
$(TEST_CAPACITY_SANITIZE_TARGET): ../test/test_capacity.c big_number.c big_number.h
	$(CC) $(SANITIZER_CFLAGS) ../test/test_capacity.c big_number.c -o $@

# Run C library tests
test_library: $(TEST_LIBRARY_TARGET)
	./$(TEST_LIBRARY_TARGET)

# Run Python ctypes tests
test_lib: $(SHARED_LIB)
	$(PYTHON) ../test/test_library.py

# Run odd_sum functional tests
test_odd_sum: $(TARGET)
	$(PYTHON) ../test/test_bignum_oddsum.py

# Run edge-case tests
test_edge: $(TARGET) $(SANITIZED_TARGET)
	$(PYTHON) ../test/test_edge.py


# Run property-based tests
test_properties: $(SHARED_LIB)
	$(PYTHON) ../test/test_properties.py

# Run capacity tests
test_capacity: $(TEST_CAPACITY_TARGET)
	./$(TEST_CAPACITY_TARGET)

# Run capacity tests with ASan and UBSan
test_capacity_sanitize: $(TEST_CAPACITY_SANITIZE_TARGET)
	ASAN_OPTIONS=halt_on_error=1:detect_leaks=1 \
	UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
	./$(TEST_CAPACITY_SANITIZE_TARGET)

# Run all test suites
test: test_library test_lib test_odd_sum test_edge \
      test_properties test_capacity test_capacity_sanitize
	@echo "All test targets completed successfully."

# Build benchmark executable
$(BENCH_TARGET): $(BENCH_SRC) big_number.c big_number.h
	$(CC) $(OPT_CFLAGS) -I. $(BENCH_SRC) big_number.c -o $@

# Run benchmark and plot results
bench: $(BENCH_TARGET)
	./$(BENCH_TARGET)
	cd ../test_bench && $(PYTHON) plot_benchmark.py

# Remove benchmark artifacts
clean-bench:
	rm -f $(BENCH_TARGET) ../test_bench/benchmark.csv

# Remove build artifacts
clean:
	rm -f $(OBJS) $(TARGET) $(SANITIZED_TARGET) \
	      $(TEST_LIBRARY_TARGET) \
	      $(TEST_CAPACITY_TARGET) \
	      $(TEST_CAPACITY_SANITIZE_TARGET) \
	      $(BENCH_TARGET) \
	      $(SHARED_LIB)
